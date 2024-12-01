from django.urls import path

from . import views

urlpatterns = [
    path('', views.our_team, name='our_team'),
    path('getSensor/<int:cnt>', views.getSensor, name='getsensor'),
    path('setSensor', views.setSensor, name='setsensor'),
    path('get_sensor_data/', views.getTableSensor, name='get_sensor_data'),
    path('index/', views.index, name='index'),
]
