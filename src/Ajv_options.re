module Format = {
  type t =
    | Fast
    | Full
    | Ignore;
  let toJson =
    fun
    | Fast => Js.Json.string("fast")
    | Full => Js.Json.string("full")
    | Ignore => Js.Json.boolean(false);
};

module UnknownFormatHandling = {
  type t =
    | Exception /* evaluates to true, the default setting */
    | Ignore(array(string)) /* ignore the formats within the given list */
    | Warn; /* Ignore all uknown formats, output a console warning. */
  let toJson =
    fun
    | Exception => Js.Json.boolean(true)
    | Ignore(a) => Js.Json.stringArray(a)
    | Warn => Js.Json.string("ignore");
};

module SchemaId = {
  type t =
    | Draft4
    | Draft6
    | Draft7
    | Any;
  let toJson =
    fun
    | Draft4 => Js.Json.string("id")
    | Draft6
    | Draft7 => Js.Json.string("$id")
    | Any => Js.Json.string("auto");
};

module RefSetting = {
  type t =
    | Exception
    | Fail
    | Ignore;
  let toJson =
    fun
    | Exception => Js.Json.boolean(true)
    | Fail => Js.Json.string("fail")
    | Ignore => Js.Json.string("ignore");
};

type t('a) = Js.t('a);

let make = () : t('a) => Js.Obj.empty();

/* Validation and Reporting options */
[@bs.set] external setData : (t('a), bool) => unit = "$data";

[@bs.set] external allErrors : (t('a), bool) => unit = "";

[@bs.set] external verbose : (t('a), bool) => unit = "";

[@bs.set] external comment : (t('a), bool) => unit = "$comment";

[@bs.set] external jsonPointers : (t('a), bool) => unit = "";

[@bs.set] external uniqueItems : (t('a), bool) => unit = "";

[@bs.set] external unicode : (t('a), bool) => unit = "";

/* formats: Js.Json.t,  Use addFormat method. */
[@bs.set] external format : (t('a), Js.Json.t) => unit = "";

[@bs.set] external unknownFormats : (t('a), Js.Json.t) => unit = "";

/* schemas: Js.Json.t,  Use addSchema method. */
/* logger: Not sure how to type this yet */
let format = (t, f) => format(t, Format.toJson(f));

let unknownFormats = (t, f) =>
  unknownFormats(t, UnknownFormatHandling.toJson(f));

/* Refernced Schema options */
[@bs.set] external schemaId : (t('a), Js.Json.t) => unit = "";

[@bs.set] external missingRefs : (t('a), Js.Json.t) => unit = "";

[@bs.set] external extendRefs : (t('a), Js.Json.t) => unit = "";

let schemaId = (t, f) => schemaId(t, SchemaId.toJson(f));

let missingRefs = (t, f) => missingRefs(t, RefSetting.toJson(f));

let extendRefs = (t, f) => extendRefs(t, RefSetting.toJson(f));

/* Data Modification options (validated data) */
[@bs.set] external removeAdditional : (t('a), bool) => unit = "";

[@bs.set] external useDefaults : (t('a), bool) => unit = "";

[@bs.set] external coerceTypes : (t('a), bool) => unit = "";

/* Asynchronous Validation options */
/* transpile: Not sure how to type this yet */
/* Advanced Options */
[@bs.set] external meta : (t('a), bool) => unit = "";

[@bs.set] external validateSchema : (t('a), bool) => unit = "";

[@bs.set] external addUsedSchema : (t('a), bool) => unit = "";

[@bs.set] external inlineRefs : (t('a), bool) => unit = "";

[@bs.set] external passContext : (t('a), bool) => unit = "";

/* loopRequired: Not sure how to type this yet */
[@bs.set] external ownProperties : (t('a), bool) => unit = "";

[@bs.set] external multipleOfPrecision : (t('a), bool) => unit = "";

[@bs.set] external messages : (t('a), bool) => unit = "";

[@bs.set] external sourceCode : (t('a), bool) => unit = "";
/* processCode: Not sure how to type this yet */
/* cache: Not sure how to type this yet */
/* serialize: Not sure how to type this yet */
