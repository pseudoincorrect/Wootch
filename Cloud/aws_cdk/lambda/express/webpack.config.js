module.exports = {
  entry: ["./dist/src/index.js"],
  target: "node",
  mode: "development",
  output: {
    path: `${process.cwd()}/dist/packed`,
    filename: "index.js",
    libraryTarget: "umd",
  },
  externals: {
    "aws-crt": "aws-crt",
  },
  module: {
    exprContextCritical: false,
  },
};
