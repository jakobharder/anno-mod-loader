# Release v11.1 preparation

Branch `release/v11` starts at PR #20's merge commit,
`9b31c2e699e922f0befb9355877866bba5f29348`.
The XML engine and pinned third-party revisions must remain identical to that
baseline. Only CLI identification, executable naming, packaging, and release
documentation change for this release.

The Anno 1800 executable is copied from the binary tracked in vscode-anno;
`anno1800/provenance.json` records its original location and checksum.
It is renamed, never rebuilt or patched.

## Local verification

- The engine, tests, WORKSPACE, submodule pins, and Bazel configuration match
  PR #20's merge commit without changes.
- A native macOS build of `xmltest117` succeeds; `--version` reports
  `xmltest117: Anno 117 v3.0`, and a simple-add patch smoke test passes.
- The native macOS test run has 107 passing and 21 failing test cases
  (48 failing assertions, concentrated in include-related cases). It is
  not a passing validation of the Windows release. No engine changes were
  made to address these failures.
- The baseline's existing [Windows and Linux CI run](https://github.com/jakobharder/anno-mod-loader/actions/runs/43457834484)
  passed `//tests/xml:xml-tests` on both platforms.
- Actionlint passes for the modified CI and release workflows.
- The frozen executable's SHA-256 and Windows x64 PE header are verified;
  packaging rejects a native macOS executable.

The new Windows executable and final archive have not been built locally.
The Windows workflow must build, test, and smoke-test them before release.

## Windows verification

With Visual Studio 2022 C++ tools, Bazelisk, Python, and recursive submodules:

```sh
bazel build //cmd/xmltest:xmltest117 -c opt
bazel test //tests/xml/... -c opt --test_output=errors
python release/package.py --binary bazel-bin/cmd/xmltest/xmltest117.exe
```

Packaging checks the frozen binary's SHA-256, both Windows x64 PE headers,
the new binary's version output, and a simple patch operation. It creates
`dist/xmltest.zip` and `dist/xmltest.zip.sha256`.
The ZIP includes both executables, the license, usage notes, and provenance.

## After authorization to push

Push the prepared branch first and wait for its Windows and Linux CI checks:

```sh
git push -u origin release/v11
```

Only after those pass, push the local annotated release tag:

```sh
git push origin v11.1
```

The tag workflow independently builds and tests that exact source, packages
both executables, and creates a **draft** GitHub release with the archive,
archive checksum, and `v11.1.md` release notes. Review its assets before
publishing. Neither pushing nor publishing is part of local preparation.
