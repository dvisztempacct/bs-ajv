open Jest;

describe("Ajv Custom Keywords", () =>
  describe("Ajv Docs: Range Keyword", () => {
    let range = Ajv.Keyword.make();
    Ajv.Keyword.setType(range, "number");
    Ajv.Keyword.setCompiler(
      range,
      (schema, parentSchema) => {
        let decode = Json.Decode.array(Json.Decode.float);
        let (min, max) =
          switch (decode(schema)) {
          | [|min, max|] => (min, max)
          | _ => failwith("range only accepts a 2 element array.")
          };
        let parentDecoder = Json.Decode.(field("exclusiveRange", boolean));
        parentDecoder(parentSchema) |> Js.to_bool ?
          x => (x > min && x < max) |> Js.Boolean.to_js_boolean :
          (x => (x >= min && x <= max) |> Js.Boolean.to_js_boolean);
      },
    );
    let options = Ajv.Options.make();
    let ajv = Ajv.ajv(options) |> Ajv.addKeyword("range", range);
    let schema =
      Json.Encode.(
        object_([
          ("range", array(Json.Encode.float, [|2.0, 4.0|])),
          ("exclusiveRange", boolean(true)),
        ])
      );
    let validate =
      switch (ajv |> Ajv.compile(schema)) {
      | `Sync(fn) => fn
      | `Async(_) => failwith("unexpected_async_mode")
      };
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    let genTest = (msg, input, expected) =>
      test(msg, () =>
        validate(Json.Encode.float(input))
        |> handler
        |> Expect.expect
        |> Expect.toBe(expected)
      );
    genTest("2.01 should pass", 2.01, true);
    genTest("3.99 should pass", 3.99, true);
    genTest("2.0 should fail", 2.0, false);
    genTest("4.0 should fail", 4.0, false);
  })
);

describe("Ajv Custom Async Keywords", () =>
  describe("Ajv Docs: checkIdExists Keyword", () => {
    let check = Ajv.Keyword.make();
    Ajv.Keyword.setType(check, "number");
    Ajv.Keyword.setIsAsync(check, true);
    Ajv.Keyword.setAsyncValidator(
      check,
      (schema, data, _) => {
        let id = Json.Decode.int(data);
        let table = Json.Decode.(field("table", string, schema));
        let result =
          switch (table) {
          | "users" => `Ok(id == 1)
          | "posts" => `Ok(id == 19)
          | _ => `Err({j|Invalid table: $table|j})
          };
        Js.(
          switch (result) {
          | `Err(msg) => Promise.reject(Failure(msg))
          | `Ok(isValid) =>
            isValid |> Boolean.to_js_boolean |> Promise.resolve
          }
        );
      },
    );
    let options = Ajv.Options.make();
    let ajv = Ajv.ajv(options) |> Ajv.addKeyword("idExists", check);
    let schema =
      Json.Encode.(
        object_([
          ("$async", boolean(true)),
          (
            "properties",
            object_([
              (
                "userId",
                object_([
                  ("type", string("integer")),
                  ("idExists", object_([("table", string("users"))])),
                ]),
              ),
              (
                "postId",
                object_([
                  ("type", string("integer")),
                  ("idExists", object_([("table", string("posts"))])),
                ]),
              ),
            ]),
          ),
        ])
      );
    let validate =
      switch (ajv |> Ajv.compile(schema)) {
      | `Sync(_) => failwith("unexpected_sync_mode")
      | `Async(fn) => fn
      };
    let genTest = (msg, userId, postId, expected) =>
      testPromise(
        msg,
        () => {
          let json =
            Json.Encode.(
              object_([("userId", int(userId)), ("postId", int(postId))])
            );
          Js.Promise.(
            validate(json)
            |> then_(
                 fun
                 | `Invalid(_) => false |> resolve
                 | `Valid(_) => true |> resolve,
               )
            |> catch(_ => resolve(false))
            |> then_(res =>
                 res |> Expect.expect |> Expect.toBe(expected) |> resolve
               )
          );
        },
      );
    genTest("valid data", 1, 19, true);
    genTest("invalid userId", 2, 19, false);
    genTest("invalid postId", 1, 20, false);
  })
);
