from django.http import HttpResponse
from .models import DetectSensor as Sens
from django.shortcuts import render
from django.core import serializers
from django.http import JsonResponse
from django.utils import timezone
import random
import json

def index(request): 
    sensor_value_list = Sens.objects.all().order_by('-reg_date').values()[:5]
    context = {
        'sensor_value_list': sensor_value_list,
    }
    return render(request, 'sensor/index.html', context)

# Create your views here.
def getSensor(request, cnt):
    results = list(Sens.objects.all().order_by('-reg_date').values())[:cnt][::-1]
    return JsonResponse(results, safe=False)

def setSensor(request):
    try:
        data = json.loads(request.body)
        Sens.objects.create(value=data['value'],reg_date = timezone.now())
        return JsonResponse({"message": "OK"}, status=200)
    except KeyError:
        return JsonResponse({"message": "KEY_ERROR"}, status=400)
