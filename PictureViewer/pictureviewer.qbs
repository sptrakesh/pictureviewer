import qbs

CppApplication
{
  name: "PictureViewer"
  cpp.cxxLanguageVersion: "c++14"
  cpp.debugInformation: qbs.buildVariant == "debug" ? true : false

  Depends
  {
    name: "Qt";
    submodules: ["core", "gui", "widgets"]
  }

  files: 
  [
    "src/**",
    "src/forms/**"
  ]

  Group
  {
    fileTagsFilter: product.type
    qbs.install: true
  }
}
