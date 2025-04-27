from django.urls import path
from .views import place_order

urlpatterns = [
    path('orders/', place_order, name='place_order'),
]
