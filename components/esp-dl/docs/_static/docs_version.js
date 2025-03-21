
var DOCUMENTATION_VERSIONS = {
  DEFAULTS: { has_targets: false,
              supported_targets: [ "esp32" ]
            },
  VERSIONS: [
    //latest
    { name: "latest", has_targets: true, supported_targets: [ "esp32", "esp32s2", "esp32s3", "esp32c3"] },

    //
    { name: "release-v1.1", has_targets: true, supported_targets: [ "esp32", "esp32s2", "esp32s3", "esp32c3"] },
  ],
  IDF_TARGETS: [
     { text: "ESP32", value: "esp32"},
     { text: "ESP32-S2", value: "esp32s2"},
     { text: "ESP32-S3", value: "esp32s3"},
     { text: "ESP32-C3", value: "esp32c3"},
  ]
};
