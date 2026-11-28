"""Check the CLI's assumed modinfo-default conversion without a game install."""

import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
import xml.etree.ElementTree as ET


class DefaultsTests(unittest.TestCase):
    def setUp(self):
        folder = tempfile.TemporaryDirectory()
        self.addCleanup(folder.cleanup)
        self.root = Path(folder.name)
        self.mod = self.root / "main mod"
        self.relative = Path("data/base/config/export/assets.xml")
        self.target = self.root / "original.xml"
        self.output = self.root / "patched.xml"
        self.write(self.target, '<AssetList><Groups><Group><Assets><Asset><Values>'
                   '<Standard><GUID>123</GUID></Standard><Storage><Amount>2</Amount></Storage>'
                   '</Values></Asset></Assets></Group></Groups></AssetList>')

    def write(self, path, text):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def modinfo(self, root, mod_id, options, filename="modinfo.json"):
        comment = '// mod metadata\n' if filename.endswith('.jsonc') else ''
        self.write(root / filename, comment + json.dumps({"ModID": mod_id, "Options": options}))

    def run_patch(self, options, body, extra=(), stdin=False, info_name="modinfo.json"):
        self.modinfo(self.mod, "my-mod", options, info_name)
        patch = '<ModOps>' + body + '</ModOps>'
        self.write(self.mod / self.relative, patch)
        args = [str(BINARY), "-v", "-m", str(self.mod), *extra]
        if stdin:
            args.extend(["-i", str(self.relative)])
        else:
            args.extend(["-o", str(self.output)])
        args.extend([str(self.target), str(self.mod / self.relative)])
        return subprocess.run(args, cwd=self.root, input=patch if stdin else None,
                              capture_output=True, text=True, timeout=30)

    def assert_success(self, result):
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_defaults_local_and_qualified_names(self):
        self.write(self.mod / "active-options.jsonc", '{"my-mod":{"amount":99}}')
        result = self.run_patch({"amount": {"default": "10"}, "factor": {"default": "1"}},
            '<ModOp GUID="123" Merge="Storage"><Amount>'
            '<ModValue Insert="$amount * $my-mod.factor" /></Amount></ModOp>')
        self.assert_success(result)
        self.assertEqual(ET.parse(self.output).findtext('.//Storage/Amount'), '10')
        self.assertIn('Default $my-mod.amount=10', result.stdout)

    def test_strict_valid_toggles(self):
        for value in [True, False, "true", "false"]:
            with self.subTest(value=value):
                result = self.run_patch({"enabled": {"type": "toggle", "default": value}},
                    '<ModOp GUID="123" Add="Storage" Condition="$enabled"><Enabled /></ModOp>')
                self.assert_success(result)
                enabled = ET.parse(self.output).find('.//Storage/Enabled') is not None
                self.assertEqual(enabled, value is True or value == "true")

    def test_invalid_toggles_fail_before_writing(self):
        for value in [1, 0, 1.0, "1", "0", "True", "False", "TRUE", " true", "false ", None, [], {}]:
            with self.subTest(value=value):
                result = self.run_patch({"enabled": {"type": "toggle", "default": value}}, '')
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('my-mod.enabled', result.stdout + result.stderr)
                self.assertIn('toggle default must be true or false', result.stdout + result.stderr)
                self.assertFalse(self.output.exists())

    def test_text_defaults_are_literals(self):
        value = 'He said "it\'s $literal"'
        result = self.run_patch({"label": {"default": value}},
            '<ModOp GUID="123" Add="Storage"><Label><ModValue Insert="$label" /></Label></ModOp>')
        self.assert_success(result)
        self.assertEqual(ET.parse(self.output).findtext('.//Storage/Label'), value)

    def test_numeric_defaults_preserve_fraction_and_exponent(self):
        for value in ["1", "-2.5", "1e-20", "1e20"]:
            with self.subTest(value=value):
                result = self.run_patch({"amount": {"default": value}},
                    '<ModOp GUID="123" Merge="Storage"><Amount>'
                    '<ModValue Insert="$amount" /></Amount></ModOp>')
                self.assert_success(result)
                self.assertEqual(float(ET.parse(self.output).findtext('.//Storage/Amount')), float(value))

    def test_unquoted_numeric_defaults_fail_before_writing(self):
        for value in [0, 1, -2.5, 1e-20, 1e20]:
            with self.subTest(value=value):
                result = self.run_patch({"amount": {"default": value}}, '')
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('my-mod.amount', result.stdout + result.stderr)
                self.assertIn('numeric default must be a quoted string', result.stdout + result.stderr)
                self.assertFalse(self.output.exists())

    def test_dependency_defaults_and_active_mod_ids(self):
        dependency = self.root / "dependency"
        self.modinfo(dependency, "dep-mod", {"amount": {"default": "7"}})
        self.write(dependency / self.relative,
            '<ModOps><ModOp GUID="123" Merge="Storage"><Amount>'
            '<ModValue Insert="$amount" /></Amount></ModOp></ModOps>')
        result = self.run_patch({},
            '<ModOp GUID="123" Add="Storage" Condition="#my-mod and #dep-mod and not(#absent-mod)">'
            '<Dependency><ModValue Insert="$dep-mod.amount" /></Dependency></ModOp>',
            extra=["-p", str(dependency)])
        self.assert_success(result)
        patched = ET.parse(self.output)
        self.assertEqual(patched.findtext('.//Storage/Amount'), '7')
        self.assertEqual(patched.findtext('.//Storage/Dependency'), '7')

    def test_defaults_in_diff_and_stdin(self):
        body = ('<ModOp GUID="123" Merge="Storage"><Amount>'
                '<ModValue Insert="$amount" /></Amount></ModOp>')
        options = {"amount": {"default": "8"}}
        result = self.run_patch(options, body, extra=["-c", "diff"])
        self.assert_success(result)
        sections = result.stdout.split('##annodiff##')
        self.assertEqual(len(sections), 3)
        self.assertIn('<Amount>8</Amount>', sections[1])
        self.assertIn('<Amount>2</Amount>', sections[2])
        result = self.run_patch(options, body, stdin=True)
        self.assert_success(result)
        self.assertEqual(ET.parse(self.root / "patched.xml").findtext('.//Storage/Amount'), '8')

    def test_invalid_default_shapes(self):
        for options in [{"amount": {}}, {"amount": {"default": None}},
                        {"amount": {"default": []}}, {"amount": {"default": {}}}]:
            with self.subTest(options=options):
                result = self.run_patch(options, '')
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('modinfo.json', result.stdout + result.stderr)
                self.assertFalse(self.output.exists())

    def test_modinfo_comments(self):
        self.modinfo(self.mod, "my-mod", {})
        text = (self.mod / "modinfo.json").read_text()
        self.write(self.mod / "modinfo.json", '// mod metadata\n' + text)
        self.write(self.mod / self.relative, '<ModOps />')
        result = subprocess.run([str(BINARY), '-m', str(self.mod), '-s', str(self.target),
                                 str(self.mod / self.relative)], cwd=self.root,
                                capture_output=True, text=True, timeout=30)
        self.assert_success(result)

    def test_jsonc_defaults_with_comments(self):
        result = self.run_patch({"amount": {"default": "12"}},
            '<ModOp GUID="123" Merge="Storage"><Amount>'
            '<ModValue Insert="$amount" /></Amount></ModOp>', info_name="modinfo.jsonc")
        self.assert_success(result)
        self.assertEqual(ET.parse(self.output).findtext('.//Storage/Amount'), '12')

    def test_jsonc_takes_precedence_over_json(self):
        self.modinfo(self.mod, "my-mod", {"amount": {"default": "99"}})
        result = self.run_patch({"amount": {"default": "12"}},
            '<ModOp GUID="123" Merge="Storage"><Amount>'
            '<ModValue Insert="$amount" /></Amount></ModOp>', info_name="modinfo.jsonc")
        self.assert_success(result)
        self.assertEqual(ET.parse(self.output).findtext('.//Storage/Amount'), '12')

    def test_invalid_jsonc_does_not_fall_back_to_json(self):
        self.write(self.mod / 'modinfo.jsonc', '{invalid}')
        result = self.run_patch({}, '')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('modinfo.jsonc', result.stdout + result.stderr)
        self.assertFalse(self.output.exists())

    def test_dependency_jsonc_defaults(self):
        dependency = self.root / 'dependency'
        self.modinfo(dependency, 'dep-mod', {"amount": {"default": "7"}}, 'modinfo.jsonc')
        self.write(dependency / self.relative,
            '<ModOps><ModOp GUID="123" Merge="Storage"><Amount>'
            '<ModValue Insert="$amount" /></Amount></ModOp></ModOps>')
        result = self.run_patch({}, '', extra=['-p', str(dependency)])
        self.assert_success(result)
        self.assertEqual(ET.parse(self.output).findtext('.//Storage/Amount'), '7')


if __name__ == "__main__":
    BINARY = Path(sys.argv.pop(1)).resolve()
    unittest.main()
