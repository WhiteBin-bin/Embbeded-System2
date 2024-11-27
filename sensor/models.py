from django.db import models
from django.utils import timezone

class DetectSensor(models.Model):
	reg_date = models.DateTimeField(editable=False)
	value = models.CharField(max_length=100,null=True)