open Jest;

describe("numeric tests", () => {
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
                ("type", string("number")),
                ("minimum", int(0)),
                ("maximum", int(9000)),
              ]),
            ),
            (
              "bar",
              object_([
                ("type", string("number")),
                ("exclusiveMinimum", int(-9000)),
                ("exclusiveMaximum", int(0)),
              ]),
            ),
          ]),
        ),
      ])
    );
  test("respected limits should validate", () => {
    let validData =
      Json.Encode.(object_([("foo", int(0)), ("bar", int(-1))]));
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(schema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(true);
  });
  test("disrespected maxima should fail to validate", () => {
    let invalidData =
      Json.Encode.(object_([("foo", int(9001)), ("bar", int(0))]));
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(schema, invalidData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(false);
  });
  test("disrespected minima should fail to validate", () => {
    let invalidData =
      Json.Encode.(object_([("foo", int(-1)), ("bar", int(-9001))]));
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(schema, invalidData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(false);
  });
  test("disrespected exclusive minima should fail to validate", () => {
    let invalidData =
      Json.Encode.(object_([("foo", int(0)), ("bar", int(-9000))]));
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(schema, invalidData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(false);
  });
  test("disrespected exclusive maxima should fail to validate", () => {
    let invalidData =
      Json.Encode.(object_([("foo", int(0)), ("bar", int(0))]));
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(schema, invalidData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(false);
  });
  test(
    "disrespected minimum and exclusiveMaximum should report correct fields",
    () => {
    let invalidData =
      Json.Encode.(object_([("foo", int(-1)), ("bar", int(0))]));
    let handler = v => {
      let handlerResult =
        switch (v) {
        | `Valid(_) => [|true, true, true|]
        | `Invalid(err) =>
          let x = Ajv.Error.toDict(err);
          [|Belt_MapString.has(x, "foo"), Belt_MapString.has(x, "bar")|];
        };
      handlerResult;
    };
    validate(schema, invalidData)
    |> handler
    |> Expect.expect
    |> Expect.toEqual([|true, true|]);
  });
  test(
    "disrespected exclusiveMinimum and maximum should report correct fields",
    () => {
    let invalidData =
      Json.Encode.(object_([("foo", int(9001)), ("bar", int(-9000))]));
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
  let multipleOfSchema =
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
                ("type", string("number")),
                ("multipleOf", int(2)),
              ]),
            ),
            (
              "bar",
              object_([
                ("type", string("number")),
                ("multipleOf", int(3)),
              ]),
            ),
          ]),
        ),
      ])
    );
  test("respected multipleOf should validate", () => {
    let validData =
      Json.Encode.(object_([("foo", int(2222)), ("bar", int(3333))]));
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(multipleOfSchema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(true);
  });
  test("disrespected multipleOf should fail to validate", () => {
    let validData =
      Json.Encode.(object_([("foo", int(3333)), ("bar", int(2222))]));
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(multipleOfSchema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(false);
  });
  test("disrespected multipleOf should report invalid fields", () => {
    let validData =
      Json.Encode.(object_([("foo", int(2222)), ("bar", int(2222))]));
    let handler =
      fun
      | `Valid(_) => [||]
      | `Invalid(err) => {
          let x = Ajv.Error.toDict(err);
          [|Belt_MapString.has(x, "foo"), Belt_MapString.has(x, "bar")|];
        };
    validate(multipleOfSchema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toEqual([|false, true|]);
  });
});
