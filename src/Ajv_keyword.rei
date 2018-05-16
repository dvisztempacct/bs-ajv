type t('a) = Js.t('a);

let make: unit => t({..});

[@bs.set] external setType : (t('a), string) => unit = "type";

[@bs.set]
external setValidator :
  (t('a), (Js.Json.t, Js.Json.t, Js.Json.t) => bool) => unit =
  "validate";

[@bs.set]
external setAsyncValidator :
  (t('a), (Js.Json.t, Js.Json.t, Js.Json.t) => Js.Promise.t(bool)) => unit =
  "validate";

[@bs.set]
external setCompiler : (t('a), (Js.Json.t, Js.Json.t, 'b) => bool) => unit =
  "compile";

[@bs.set]
external setAsyncCompiler :
  (t('a), (Js.Json.t, Js.Json.t, 'b) => Js.Promise.t(bool)) => unit =
  "compile";

[@bs.set]
external setMacro : (t('a), (Js.Json.t, Js.Json.t) => Js.Json.t) => unit =
  "macro";

[@bs.set]
external setInline :
  (t('a), (Js.Json.t, Js.Json.t, Js.Json.t) => Js.Json.t) => unit =
  "inline";

[@bs.set]
external setMetaSchema :
  (t('a), [@bs.unwrap] [ | `Inline(Js.Json.t) | `Ref(string)]) => unit =
  "metaSchema";

[@bs.set] external setModifying : (t('a), bool) => unit = "modifying";

[@bs.set] external setValidationResult : (t('a), bool) => unit = "valid";

[@bs.set] external setHasDataReference : (t('a), bool) => unit = "$data";

[@bs.set] external setIsAsync : (t('a), bool) => unit = "async";
