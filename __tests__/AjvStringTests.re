open Jest;

describe("string tests", () => {
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
            (
              "foo",
              object_([
                ("type", string("string")),
                ("maxLength", int(5)),
              ]),
            ),
            (
              "bar",
              object_([
                ("type", string("string")),
                ("minLength", int(1)),
                ("pattern", string("^(e.*|)$")),
              ]),
            ),
          ]),
        ),
      ])
    );
  test("respected limits should validate", () => {
    let validData =
      Json.Encode.(
        object_([("foo", string("hello")), ("bar", string("eugenia"))])
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
  test("disrespected limits should fail to validate", () => {
    let validData =
      Json.Encode.(
        object_([
          ("foo", string("greetings")),
          ("bar", string("eugenia")),
        ])
      );
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(schema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(false);
  });
  test("disrespected limits should report invalid fields", () => {
    let validData =
      Json.Encode.(
        object_([("foo", string("hello")), ("bar", string(""))])
      );
    let handler =
      fun
      | `Valid(_) => [||]
      | `Invalid(err) => {
          let x = Ajv.Error.toDict(err);
          [|Belt_MapString.has(x, "foo"), Belt_MapString.has(x, "bar")|];
        };
    validate(schema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toEqual([|false, true|]);
  });
  test("disrespected regexp pattern should fail to validate", () => {
    let invalidData =
      Json.Encode.(
        object_([("foo", string("hello")), ("bar", string("gena"))])
      );
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
    |> Expect.toEqual([|false, true|]);
  });
});
