from datetime import datetime


def pretty_date(time):
    now = datetime.now()
    diff = now - time
    second_diff = diff.seconds
    day_diff = diff.days

    if day_diff == 0:
        if second_diff < 10:
            return "just now"
        if second_diff < 60:
            return f"{second_diff} seconds ago"
        if second_diff < 120:
            return "a minute ago"
        if second_diff < 3600:
            return f"{second_diff // 60} minutes ago"
        if second_diff < 7200:
            return "an hour ago"
        if second_diff < 86400:
            return f"{second_diff // 3600} hours ago"
    if day_diff == 1:
        return "yesterday"
    if day_diff < 7:
        return f"{day_diff} days ago"
    if day_diff < 31:
        return f"{day_diff // 7} weeks ago"
    if day_diff < 365:
        return f"{day_diff // 30} months ago"
    return f"{day_diff // 365} years ago"
