from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('getSensor/<int:cnt>', views.getSensor, name='getsensor'),
    path('setSensor', views.setSensor, name='setsensor'),
]
