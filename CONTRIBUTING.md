# Contributing

## Game branches and xmltest

`release/1800` maintains Anno 1800 (loader 10); `release/117` maintains
Anno 117 (loader 11). These branch names identify games, so they remain valid
if a game adopts a newer loader. Major feature improvements advance the loader
major version. xmltest has no independent version sequence.

The build targets are `//cmd/xmltest:xmltest1800` and
`//cmd/xmltest:xmltest117` in their respective branches. Windows executables
use the corresponding `.exe` names. `--version` identifies the game and loader.
Historical tags and the previous `release/v11` branch are retained.

The current local `release/117` checkout starts from `main` at `44f0beb`,
including modinfo defaults and the recent XML fixes. The separate 1800 worktree
starts from `origin/stable/anno1800` at `8326b7c`. Its patch engine is unchanged;
only CLI identification, build naming, and packaging configuration change.
That source reference is not claimed to be the exact source of the historical
distributed Windows binary, whose original build commit is unknown.

The latest game's release package includes both Windows executables.
`release/package.py` builds the archive using the freshly built 117 tool and
the preserved 1800 binary in `release/anno1800/`. The preserved binary is not
rebuilt; its checksum and origin are recorded beside it. Building the 1800
branch is for testing and future maintenance and does not replace that binary.

```sh
python release/package.py --binary bazel-bin/cmd/xmltest/xmltest117.exe
```

## Local VS Code testing

The sibling checkouts are `anno-mod-loader` (`release/117`),
`anno-mod-loader-1800` (`release/1800`), and `vscode-anno`.
On Windows, build the appropriate Bazel target in each loader checkout and
copy the resulting `.exe` into `vscode-anno/external/`. Keep the preserved
Windows 1800 executable unless deliberately testing a rebuilt version.

For native macOS/Linux development, initialize each checkout's pinned
pugixml/spdlog submodules (and json for 117), then run from their parent folder:

```sh
python3 anno-mod-loader/tools/build_xmltest.py --output vscode-anno/external/xmltest117
python3 anno-mod-loader/tools/build_xmltest.py --source anno-mod-loader-1800 --output vscode-anno/external/xmltest1800
```

The native 1800 build also requires installed Abseil headers because its
original CLI includes them. These native tools are local development artifacts;
they do not modify or replace either tracked Windows executable. The extension
selects `.exe` on Windows and the extensionless native tools on macOS/Linux.

In `vscode-anno`, run `npm run compile`, open that folder in VS Code, and use
the locally prepared **Run Anno Modding Tools** launch configuration (F5).
Use **Run Tests**, **Show Diff**, live ModOp diagnostics, and asset definitions
in mods for both games. The mod metadata selects the executable; asset views
use the game of the requested document. Configure extracted game data as usual
(`anno.rdaFolder` for 1800 and `anno.117.gamePath` for 117).

The pre-existing Windows binaries remain the published v11.1 tools; testing
new 117 source changes on Windows requires a Windows rebuild. The native tools
and extension bundle are built locally for this checkout.

## Tests

Initialize the pinned dependencies with `git submodule update --init --recursive`.
Use Bazelisk to select the repository's Bazel version.

On Windows, install the Visual Studio 2022 C++ tools, then run:

```sh
bazel build //... -c opt
bazel test //tests/xml/... //cmd/xmltest/... -c opt --test_output=errors
```

On Linux, use the Linux configuration:

```sh
bazel --noworkspace_rc --bazelrc=.linux.bazelrc build //cmd/... //tests/... -c opt
bazel --noworkspace_rc --bazelrc=.linux.bazelrc test //tests/xml/... //cmd/xmltest/... -c opt --test_output=errors
```

## Commit Messages

Use [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/):

```text
type(scope): short description
```

Common types: `feat`, `fix`, `refactor`, `docs`, `test`, `chore`, `build`, `ci`.
Scope is optional. Use an area such as `xml-operations`, `xmltest` or `filedb`.
Describe the change in imperative form, without a trailing period.

For squash merges, use this convention for the PR title and resulting commit.
