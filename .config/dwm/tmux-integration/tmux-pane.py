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


def run_proc_distro(distro_name, tmux):
    process = run(
        f"distrobox enter {distro_name} -e tmux ls -F {tmux}",
        shell=True,
        capture_output=True,
    )
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


def get_pane_distro(distro_name, pane_id):
    process = run(
        f"distrobox enter {distro_name} -e tmux capture-pane -pt {pane_id}",
        shell=True,
        capture_output=True,
    )
    return map(
        lambda x: x.decode("utf-8"),
        filter(lambda x: len(x) > 0, process.stdout.split(b"\n")),
    )


def get_all_boxes():
    process = run(
        "distrobox ls | awk 'NR>1' | awk '{print $3}'", shell=True, capture_output=True
    )
    return map(
        lambda x: x.decode("utf-8"),
        filter(lambda x: len(x) > 0, process.stdout.split(b"\n")),
    )


name = [f"'{i}'" for i in run_proc(TMUX_NAME)]
pane_id = run_proc(TMUX_PANE_ID)
boxes = list(get_all_boxes())

sessions = []
sessions.append(
    [
        "CREATE NEW SESSION",
        "Create a new session with a given name.\nIf no name is given, a random one will be given.",
    ]
)
sessions.append(["NONE", "Enter without tmux session."])


pane = sys.argv[1].split("|")[0].strip()

for box in boxes:
    if f"BOX: {box}" == pane:
        pane_id_distro = list(run_proc_distro(box, TMUX_PANE_ID))
        if len(pane_id_distro) == 0:
            sessions.append([f"BOX: {box}", "A distrobox container"])
            continue
        pane_text = list(get_pane_distro(box, pane_id_distro[0]))
        sessions.append([f"BOX: {box}", f"{"\n".join(pane_text)}"])

for n, p in zip(name, pane_id):
    sessions.append(
        [
            n,
            p,
        ]
    )

pane_id = list(
    map(
        lambda x: x[1],
        filter(lambda x: x[0] == pane, sessions),
    )
)
if len(pane_id) == 0:
    pane_id = list(
        map(
            lambda x: x[1],
            filter(lambda x: x[0].replace("'", "") == pane, sessions),
        )
    )[0]
else:
    pane_id = pane_id[0]

if pane_id[0] == "%":
    print("\n".join(get_pane(pane_id)))
else:
    print(pane_id)
