import sys
from subprocess import run

ACTION = sys.argv[2]
SELECT = sys.argv[1].split("|")[0].strip()


def delete_pane():
    run(f"tmux kill-session -t {SELECT}", shell=True)


if ACTION == "DELETE":
    delete_pane()
