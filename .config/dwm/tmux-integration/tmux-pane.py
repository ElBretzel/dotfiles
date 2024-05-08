from subprocess import run
import sys

TMUX_NAME = "'#{session_name}'"
TMUX_PANE_ID = "'#{pane_id}'"


def run_proc(tmux):
    process = run(f"tmux ls -F {tmux}", shell=True, capture_output=True)
    return map(
        lambda x: x.decode("utf-8"),
        filter(lambda x: len(x) > 0, process.stdout.split(b"\n")),
    )


def get_pane(pane_id):
    process = run(f"tmux capture-pane -pt {pane_id}", shell=True, capture_output=True)
    return map(
        lambda x: x.decode("utf-8"),
        filter(lambda x: len(x) > 0, process.stdout.split(b"\n")),
    )


name = [f"'{i}'" for i in run_proc(TMUX_NAME)]
pane_id = run_proc(TMUX_PANE_ID)

sessions = []
sessions.append(
    [
        "CREATE NEW SESSION",
        "Create a new session with a given name.\nIf no name is given, a random one will be given.",
    ]
)
sessions.append(["NONE", "Enter without tmux session."])

for n, p in zip(name, pane_id):
    sessions.append(
        [
            n,
            p,
        ]
    )

pane = sys.argv[1].split("|")[0].strip()
pane_id = list(
    map(
        lambda x: x[1],
        filter(lambda x: x[0] == pane, sessions),
    )
)[0]

if pane_id[0] == "%":
    print("\n".join(get_pane(pane_id)))
else:
    print(pane_id)
