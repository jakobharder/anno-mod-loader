# Contributing

## Tests

Initialize the pinned dependencies with `git submodule update --init --recursive`.
Use Bazelisk to select the repository's Bazel version.

On Windows, install the Visual Studio 2022 C++ tools, then run:

```sh
bazel build //... -c opt
bazel test //tests/xml/... //cmd/xmltest:prepatch-tests -c opt --test_output=errors
```

On Linux, use the Linux configuration:

```sh
bazel --noworkspace_rc --bazelrc=.linux.bazelrc build //cmd/... //tests/... -c opt
bazel --noworkspace_rc --bazelrc=.linux.bazelrc test //tests/xml/... //cmd/xmltest:prepatch-tests -c opt --test_output=errors
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
