from django import template

register = template.Library()

@register.filter
def split(value, delimiter=", "):
    if not isinstance(value, str):
        return []
    
    # Split by the delimiter, extract numbers, and map to 'OFF'/'ON'
    return [
        "OFF" if int(pair.split(":")[1]) == 0 else "ON"
        for pair in value.split(delimiter)
    ]