type t = {
  key: string,
  message: string,
};

/* NOTE we offer ml-native representations of quite a lot of the original
 * Ajv error description objects, however we don't use much of them at all.
 * They are left here for you if you should have need for them, but be
 * warned they may not be thoroughly tested!
 * RawValidationError.RawParams.fromJson is a particularly juicy target if
 * you want to start cutting the fat
 */
module RawValidationError = {
  module RawParams = {
    type t =
      | MissingProperty(string)
      | LimitError(int)
      | TypeError(string)
      | MultipleOfError(int)
      | PatternError(string)
      | ContainsError;
    let fromJson = (keyword, json) =>
      switch (keyword) {
      | "required" =>
        MissingProperty(Json.Decode.(field("missingProperty", string, json)))
      | "type" => TypeError(Json.Decode.(field("type", string, json)))
      | "minimum"
      | "maximum"
      | "exclusiveMaximum"
      | "exclusiveMinimum"
      | "maxLength"
      | "minLength"
      | "maxItems"
      | "minItems"
      | "maxProperties"
      | "minProperties" =>
        LimitError(Json.Decode.(field("limit", int, json)))
      | "pattern" =>
        PatternError(Json.Decode.(field("pattern", string, json)))
      | "multipleOf" =>
        MultipleOfError(Json.Decode.(field("multipleOf", int, json)))
      | "contains" => ContainsError
      | _ => failwith({j|Unknown keyword: $keyword|j})
      };
  };
  type t = {
    keyword: string,
    dataPath: string,
    schemaPath: string,
    params: RawParams.t,
    message: string,
    schema: option(string),
    parentSchema: option(string),
  };
  let fromJson = json => {
    let keyword = Json.Decode.field("keyword", Json.Decode.string, json);
    Json.Decode.{
      keyword,
      dataPath: json |> field("dataPath", string),
      schemaPath: json |> field("schemaPath", string),
      params: json |> field("params", RawParams.fromJson(keyword)),
      message: json |> field("message", string),
      schema: json |> optional(field("schema", string)),
      parentSchema: json |> optional(field("parentSchema", string)),
    };
  };
  /* See RFC6901 for details on escaping JSON Pointers */
  let unescapeJsonPointerRegexp =
    Js.Re.fromStringWithFlags({|~[01]|}, ~flags="g");
  let unescapeJsonPointerEscapedSubstr = (s, _, _) =>
    s.[1] == '0' ? "~" : "/";
  let unescapeJsonPointerStr = s =>
    Js.String.unsafeReplaceBy0(
      unescapeJsonPointerRegexp,
      unescapeJsonPointerEscapedSubstr,
      s,
    );
  /* Derivation of a flat field name from dataPath applies to most
   * situations. Notably it does not apply for "required" keyword, and has
   * a special case for any keyword that fails inside an array.
   */
  let dataPathToFieldNameRegexp = Js.Re.fromString({|^/(.*?)(/[^/]+)?$|});
  let dataPathToFieldName = dataPath =>
    switch (Js.Re.exec(dataPath, dataPathToFieldNameRegexp)) {
    | None => failwith({j|nonconformant Ajv dataPath $dataPath|j})
    | Some(result) =>
      switch (Js.Re.captures(result)[1] |> Js.Nullable.toOption) {
      | None => failwith({j|nonconformant Ajv dataPath $dataPath|j})
      | Some(result) => result |> unescapeJsonPointerStr
      }
    };
};

let toError = ({keyword, dataPath, message, params, _}: RawValidationError.t) =>
  switch (keyword, params) {
  | ("required", MissingProperty(key)) => {key, message}
  | ("type", TypeError(_))
  | ("minimum", LimitError(_))
  | ("maximum", LimitError(_))
  | ("maxLength", LimitError(_))
  | ("minLength", LimitError(_))
  | ("exclusiveMinimum", LimitError(_))
  | ("exclusiveMaximum", LimitError(_))
  | ("maxItems", LimitError(_))
  | ("minItems", LimitError(_))
  | ("maxProperties", LimitError(_))
  | ("minProperties", LimitError(_))
  | ("pattern", PatternError(_))
  | ("contains", ContainsError)
  | ("multipleOf", MultipleOfError(_)) => {
      key: RawValidationError.dataPathToFieldName(dataPath),
      message,
    }
  | _ => failwith({j|Unknown keyword: $keyword|j})
  };

let toDict = json =>
  Json.Decode.list(RawValidationError.fromJson, json)
  |> Belt_List.map(_, toError)
  |> Belt_List.reduce(_, Belt_MapString.empty, (m, e) =>
       Belt_MapString.set(m, e.key, e.message)
     );

let logDict = dict =>
  Belt_MapString.forEach(
    dict,
    (k, v) => {
      Js.log("kv");
      Js.log(k);
      Js.log(v);
      ();
    },
  );
