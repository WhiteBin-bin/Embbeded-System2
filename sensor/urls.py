from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('getSensor/<int:cnt>', views.getSensor, name='getsensor'),
    path('setSensor', views.setSensor, name='setsensor'),

    path('cane-description/', views.cane_description, name='cane_description'),
    path('our-team/', views.our_team, name='our_team'),
]
