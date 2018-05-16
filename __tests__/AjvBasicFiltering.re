open Jest;

describe("data filtering (removeAdditional option)", () => {
  let options = Ajv_options.make();
  Ajv_options.allErrors(options, true);
  Ajv_options.jsonPointers(options, true);
  Ajv_options.removeAdditional(options, true);
  let validate = (schema, document) => {
    let validate_ajv =
      switch (Ajv.ajv(options) |> Ajv.compile(schema)) {
      | `Sync(fn) => fn
      | `Async(_) => failwith("unexpected_async_mode")
      };
    validate_ajv(document);
  };
  let schema =
    Json.Encode.(
      object_([
        ("required", array(string, [|"foo", "bar"|])),
        ("additionalProperties", bool(false)),
        (
          "properties",
          object_([
            ("foo", object_([("type", string("number"))])),
            (
              "bar",
              object_([("baz", object_([("type", string("string"))]))]),
            ),
          ]),
        ),
      ])
    );
  /* https://www.npmjs.com/package/ajv#filtering-data */
  test("basic remove additional test", () => {
    let validData =
      Json.Encode.(
        object_([
          ("foo", int(0)),
          ("additional1", int(1)),
          (
            "bar",
            object_([("baz", string("abc")), ("additional2", int(2))]),
          ),
        ])
      );
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(schema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(true);
  });
  test("errors should be returned as a json array", () => {
    let invalidData = Json.Encode.(object_([("foo", string("bar"))]));
    let handler =
      fun
      | `Valid(_) => false
      | `Invalid(err) =>
        switch (Js.Json.classify(err)) {
        | Js.Json.JSONArray(_) => true
        | x =>
          Js.log2("INVALID: ", x);
          false;
        };
    validate(schema, invalidData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(true);
  });
  test("required errors should all be returned", () => {
    let invalidData = Json.Encode.(object_([]));
    let handler =
      fun
      | `Valid(_) => [||]
      | `Invalid(err) => {
          let x = Ajv.Error.toDict(err);
          [|Belt_MapString.has(x, "foo"), Belt_MapString.has(x, "bar")|];
        };
    validate(schema, invalidData)
    |> handler
    |> Expect.expect
    |> Expect.toEqual([|true, true|]);
  });
});
