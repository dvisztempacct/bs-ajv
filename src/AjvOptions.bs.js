// Generated by BUCKLESCRIPT VERSION 2.2.2, PLEASE EDIT WITH CARE
'use strict';


function toJson(param) {
  switch (param) {
    case 0 : 
        return "fast";
    case 1 : 
        return "full";
    case 2 : 
        return false;
    
  }
}

var Format = /* module */[/* toJson */toJson];

function toJson$1(param) {
  if (typeof param === "number") {
    if (param !== 0) {
      return "ignore";
    } else {
      return true;
    }
  } else {
    return param[0];
  }
}

var UnknownFormatHandling = /* module */[/* toJson */toJson$1];

function toJson$2(param) {
  if (param !== 0) {
    if (param >= 3) {
      return "auto";
    } else {
      return "$id";
    }
  } else {
    return "id";
  }
}

var SchemaId = /* module */[/* toJson */toJson$2];

function toJson$3(param) {
  switch (param) {
    case 0 : 
        return true;
    case 1 : 
        return "fail";
    case 2 : 
        return "ignore";
    
  }
}

var RefSetting = /* module */[/* toJson */toJson$3];

function make() {
  return { };
}

function format(t, f) {
  t.format = toJson(f);
  return /* () */0;
}

function unknownFormats(t, f) {
  t.unknownFormats = toJson$1(f);
  return /* () */0;
}

function schemaId(t, f) {
  t.schemaId = toJson$2(f);
  return /* () */0;
}

function missingRefs(t, f) {
  t.missingRefs = toJson$3(f);
  return /* () */0;
}

function extendRefs(t, f) {
  t.extendRefs = toJson$3(f);
  return /* () */0;
}

exports.Format = Format;
exports.UnknownFormatHandling = UnknownFormatHandling;
exports.SchemaId = SchemaId;
exports.RefSetting = RefSetting;
exports.make = make;
exports.format = format;
exports.unknownFormats = unknownFormats;
exports.schemaId = schemaId;
exports.missingRefs = missingRefs;
exports.extendRefs = extendRefs;
/* No side effect */