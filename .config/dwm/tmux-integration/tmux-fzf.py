from subprocess import run
from datetime import datetime
from pretty import pretty_date

TMUX_NAME = "'#{session_name}'"
TMUX_CREATED = "'#{session_created}'"
TMUX_ACTIVITY = "'#{session_activity}'"


def run_proc(tmux):
    process = run(f"tmux ls -F {tmux}", shell=True, capture_output=True)
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


boxes = list(get_all_boxes())
name = [f"'{i}'" for i in run_proc(TMUX_NAME)]
created = run_proc(TMUX_CREATED)
activity = run_proc(TMUX_ACTIVITY)

sessions = []
options = -4 - len(boxes) - 1
length = 0
sessions.append([options := options + 1, "(to show all bindings, press ctrl-h)"])
sessions.append([options := options + 1, "Choose a TMUX session"])
sessions.append([options := options + 1, "NONE"])
sessions.append([options := options + 1, "CREATE NEW SESSION"])
for i in range(len(boxes)):
    sessions.append([options := options + 1, f"BOX: {boxes[i]}"])
if len(name) > 0:
    length = max(map(lambda x: len(x), name))

for n, c, a in zip(name, created, activity):
    created_ago = pretty_date(datetime.fromtimestamp(int(c)))
    activity_ago = pretty_date(datetime.fromtimestamp(int(a)))
    sessions.append(
        [
            int(a),
            f"{n : <{length + 1}} | created {created_ago} and was last used {activity_ago}",
        ]
    )
sorted_session = "\n".join(map(lambda x: x[1], sorted(sessions, key=lambda x: x[0])))
print(sorted_session)
