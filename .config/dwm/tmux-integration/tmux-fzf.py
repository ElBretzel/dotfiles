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


name = [f"'{i}'" for i in run_proc(TMUX_NAME)]
created = run_proc(TMUX_CREATED)
activity = run_proc(TMUX_ACTIVITY)

sessions = []
length = 0
sessions.append([-2, "CREATE NEW SESSION"])
sessions.append([-1, "NONE"])
if len(name) > 0:
    length = max(map(lambda x: len(x), name))

for n, c, a in zip(name, created, activity):
    created_ago = pretty_date(datetime.fromtimestamp(int(c)))
    activity_ago = pretty_date(datetime.fromtimestamp(int(a)))
    sessions.append(
        [
            int(a),
            f"{n : <{length + 1}} | Created: {created_ago} Used: {activity_ago}",
        ]
    )
sorted_session = "\n".join(map(lambda x: x[1], sorted(sessions, key=lambda x: x[0])))
print(sorted_session)
