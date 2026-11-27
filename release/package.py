"""Package the pinned Anno 1800 binary and a freshly built Anno 117 binary."""

import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import tempfile
import xml.etree.ElementTree as ET
import zipfile


ROOT = Path(__file__).resolve().parents[1]
BASELINE = "9b31c2e699e922f0befb9355877866bba5f29348"


def digest(data):
    return hashlib.sha256(data).hexdigest()


def check_windows_x64(data):
    if len(data) < 64 or data[:2] != b"MZ":
        raise ValueError("Expected a Windows x64 executable")
    offset = struct.unpack_from("<I", data, 0x3C)[0]
    if data[offset:offset + 6] != b"PE\0\0\x64\x86":
        raise ValueError("Expected a Windows x64 PE header")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", required=True, type=Path)
    parser.add_argument("--output", type=Path, default=ROOT / "dist/xmltest.zip")
    args = parser.parse_args()
    binary = args.binary.resolve()

    old_info = json.loads((ROOT / "release/anno1800/provenance.json").read_text())
    old_bytes = (ROOT / "release/anno1800/xmltest1800.exe").read_bytes()
    if digest(old_bytes) != old_info["sha256"]:
        raise ValueError("Frozen Anno 1800 executable checksum mismatch")
    new_bytes = binary.read_bytes()
    for data in (old_bytes, new_bytes):
        check_windows_x64(data)

    # Validate the actual Windows binary before distributing it.
    version = subprocess.run([str(binary), "--version"], check=True,
                             capture_output=True, text=True).stdout.strip()
    if version != "xmltest117: Anno 117 v3.0":
        raise ValueError(f"Unexpected implementation version: {version!r}")
    with tempfile.TemporaryDirectory() as folder:
        output = Path(folder) / "patched.xml"
        subprocess.run([
            str(binary), "-o", str(output),
            str(ROOT / "tests/xml/legacy-add/simple_add_input.xml"),
            str(ROOT / "tests/xml/legacy-add/simple_add_patch.xml"),
        ], cwd=ROOT, check=True)
        if ET.parse(output).find("./Node/Cat") is None:
            raise ValueError("Packaged executable failed the simple-add smoke test")

    commit = subprocess.check_output(
        ["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip()
    manifest = {
        "release": "v11.1",
        "xmltest1800": old_info,
        "xmltest117": {
            "filename": "xmltest117.exe",
            "game_implementation": "Anno 117 v3.0",
            "loader_baseline": BASELINE,
            "build_commit": commit,
            "sha256": digest(new_bytes),
        },
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(args.output, "w", zipfile.ZIP_DEFLATED) as archive:
        archive.writestr("xmltest1800.exe", old_bytes)
        archive.writestr("xmltest117.exe", new_bytes)
        archive.write(ROOT / "LICENSE", "LICENSE")
        archive.write(ROOT / "release/README.txt", "README.txt")
        archive.writestr("provenance.json", json.dumps(manifest, indent=2) + "\n")
    checksum = args.output.with_name(args.output.name + ".sha256")
    checksum.write_text(f"{digest(args.output.read_bytes())}  {args.output.name}\n")
    print(f"Created {args.output} and {checksum}")


if __name__ == "__main__":
    main()
