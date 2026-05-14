from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]
TESTCASE_DIR = ROOT / "testcases"


def scheduler_path() -> Path:
    candidates = [ROOT / "lab6", ROOT / "lab6.exe"]
    for candidate in candidates:
        if candidate.exists():
            return candidate
    raise FileNotFoundError("Build output not found. Run `make build` first.")


def normalize(text: str) -> str:
    return "\n".join(line.rstrip() for line in text.replace("\r\n", "\n").split("\n")).strip()


def main() -> int:
    binary = scheduler_path()
    failures: list[str] = []

    for input_file in sorted(TESTCASE_DIR.glob("*-input.txt")):
        expected_file = TESTCASE_DIR / input_file.name.replace("-input.txt", "-output.txt")
        if not expected_file.exists():
            failures.append(f"{input_file.name}: missing expected output file")
            continue

        result = subprocess.run(
            [str(binary)],
            input=input_file.read_text(),
            text=True,
            capture_output=True,
            cwd=ROOT,
        )

        if result.returncode != 0:
            failures.append(f"{input_file.name}: exited with {result.returncode}: {result.stderr}")
            continue

        expected = expected_file.read_text()
        if normalize(result.stdout) != normalize(expected):
            failures.append(f"{input_file.name}: output mismatch")

    if failures:
        print("Fixture test failures:")
        for failure in failures:
            print(f"- {failure}")
        return 1

    print("All CPU scheduling fixture tests passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())

