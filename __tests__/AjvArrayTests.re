open Jest;

describe("array tests", () => {
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
        ("required", array(string, [|"foo"|])),
        ("additionalProperties", bool(false)),
        (
          "properties",
          object_([
            (
              "foo",
              object_([
                ("type", string("array")),
                ("minItems", int(1)),
                ("maxItems", int(3)),
                ("uniqueItems", bool(true)),
                (
                  "items",
                  object_([
                    ("type", string("string")),
                    ("maxLength", int(10)),
                  ]),
                ),
              ]),
            ),
          ]),
        ),
      ])
    );
  let containsSchema =
    Json.Encode.(
      object_([
        ("required", array(string, [|"foo"|])),
        ("additionalProperties", bool(false)),
        (
          "properties",
          object_([
            (
              "foo",
              object_([
                ("type", string("array")),
                (
                  "contains",
                  object_([
                    ("type", string("string")),
                    ("minLength", int(10)),
                  ]),
                ),
              ]),
            ),
          ]),
        ),
      ])
    );
  /* TODO how do i represent a heterogeneous js array to Json.Encode? */
  test("respected array limits should validate", () => {
    let validData =
      Json.Encode.(
        object_([("foo", array(string, [|"one", "two", "three"|]))])
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
  test("disrespected array limits should fail to validate", () => {
    let validData =
      Json.Encode.(
        object_([
          ("foo", array(string, [|"one", "two", "three", "four"|])),
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
  test("disrespected array limits should fail to report invalid fields", () => {
    let validData =
      Json.Encode.(
        object_([
          ("foo", array(string, [|"one", "two", "three", "four"|])),
        ])
      );
    let handler = v => {
      let handlerResult =
        switch (v) {
        | `Valid(_) => false
        | `Invalid(err) =>
          let x = Ajv.Error.toDict(err);
          Belt_MapString.has(x, "foo") ? true : false;
        };
      handlerResult;
    };
    validate(schema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(true);
  });
  test("disrespected array item type should fail to validate", () => {
    let validData =
      Json.Encode.(object_([("foo", array(int, [|1, 2, 3, 4|]))]));
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(schema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(false);
  });
  test("disrespected array item type should indicate invalid field", () => {
    let validData =
      Json.Encode.(
        object_([
          ("foo", array(string, [|"one", "two", "threeeeeeeeeeeeeeeee"|])),
        ])
      );
    let handler = v => {
      let handlerResult =
        switch (v) {
        | `Valid(_) => false
        | `Invalid(err) =>
          let x = Ajv.Error.toDict(err);
          Belt_MapString.has(x, "foo") ? true : false;
        };
      handlerResult;
    };
    validate(schema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(true);
  });
  test("respected contains keyword should validate", () => {
    let validData =
      Json.Encode.(
        object_([
          ("foo", array(string, [|"one", "two", "threeeeeeeeeeeeeeeee"|])),
        ])
      );
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(containsSchema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(true);
  });
  test("disrespected contains keyword should fail to validate", () => {
    let validData =
      Json.Encode.(
        object_([("foo", array(string, [|"one", "two", "three"|]))])
      );
    let handler =
      fun
      | `Valid(_) => true
      | `Invalid(_) => false;
    validate(containsSchema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(false);
  });
  test("disrespected contains keyword should indicate invalid field", () => {
    let validData =
      Json.Encode.(
        object_([("foo", array(string, [|"one", "two", "three"|]))])
      );
    let handler = v => {
      let handlerResult =
        switch (v) {
        | `Valid(_) => false
        | `Invalid(err) =>
          let x = Ajv.Error.toDict(err);
          Belt_MapString.has(x, "foo") ? true : false;
        };
      handlerResult;
    };
    validate(containsSchema, validData)
    |> handler
    |> Expect.expect
    |> Expect.toBe(true);
  });
});
