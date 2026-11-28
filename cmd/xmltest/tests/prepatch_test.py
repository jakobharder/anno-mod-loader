"""Exercise dependency patch discovery through the actual CLI."""

from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
import xml.etree.ElementTree as ET


class PrepatchTests(unittest.TestCase):
    def setUp(self):
        folder = tempfile.TemporaryDirectory()
        self.addCleanup(folder.cleanup)
        self.root = Path(folder.name)

    def write(self, path, text):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def check_prepatch(self, patch_path, dependency_path, legacy_path=None):
        target = self.root / "original.xml"
        output = self.root / "patched.xml"
        mod = self.root / "main mod"
        self.write(target, "<Test><Value>original</Value></Test>")
        self.write(mod / patch_path,
                   '<ModOps><ModOp Type="add" Path="/Test"><Main /></ModOp></ModOps>')

        # The later dependency must win, and an absent dependency must be skipped.
        dependencies = [self.root / "missing", self.root / "dependency a",
                        self.root / "dependency b"]
        dependencies[0].mkdir()
        for dependency, value in zip(dependencies[1:], ["first", "second"]):
            patch = ('<ModOps><ModOp Type="replace" Path="/Test/Value">'
                     f'<Value>{value}</Value></ModOp></ModOps>')
            self.write(dependency / dependency_path, patch)
            if legacy_path:
                self.write(dependency / legacy_path,
                           patch.replace(value, "wrong-game-layout"))

        args = [str(BINARY)]
        for dependency in dependencies:
            args.extend(["-p", str(dependency)])
        args.extend(["-m", str(mod), "-o", str(output),
                     str(target), str(mod / patch_path)])
        result = subprocess.run(args, cwd=self.root, capture_output=True,
                                text=True, timeout=30)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        patched = ET.parse(output)
        self.assertEqual(patched.findtext("Value"), "second",
                         result.stdout + result.stderr)
        self.assertIsNotNone(patched.find("Main"))
        self.assertEqual(result.stdout.count("Prepatch:"), 2)
        self.assertEqual(ET.parse(target).findtext("Value"), "original")

    def test_anno117_assets(self):
        self.check_prepatch(
            "data/base/config/export/assets.xml",
            "data/base/config/export/assets.xml",
            "data/config/export/main/asset/assets.xml")

    def test_anno117_templates(self):
        self.check_prepatch(
            "data/base/config/export/templates.xml",
            "data/base/config/export/templates.xml",
            "data/config/export/main/asset/templates.xml")

    def test_asset_fragment_uses_main_dependency_patch(self):
        self.check_prepatch("features/feature.include.xml",
                            "data/base/config/export/assets.xml")

    def test_cfg_sidecar_keeps_relative_path(self):
        self.check_prepatch("data/graphics/scene.cfg.xml",
                            "data/graphics/scene.cfg.xml")

    def test_infotip_sidecar_keeps_relative_path(self):
        self.check_prepatch("data/infotips/export.bin.xml",
                            "data/infotips/export.bin.xml")


if __name__ == "__main__":
    BINARY = Path(sys.argv.pop(1)).resolve()
    unittest.main()
