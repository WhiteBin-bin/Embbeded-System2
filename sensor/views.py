from django.http import HttpResponse
from .models import DetectSensor as Sens
from django.shortcuts import render
from django.core import serializers
from django.http import JsonResponse
from django.utils import timezone
import random
import json

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

def getTableSensor(request):
    sensor_data = Sens.objects.all().order_by('-reg_date').values('id', 'value')[:14]
    data = list(sensor_data)  # QuerySet을 리스트로 변환
    return JsonResponse(data, safe=False)

def cane_description(request):
    return render(request, 'sensor/cane_description.html')

def our_team(request):
    return render(request, 'sensor/our_team.html')

def index(request): 
    return render(request, 'sensor/index.html')