from rest_framework.decorators import api_view
from rest_framework.response import Response
from .models import Table, MenuItem, Order, OrderItem
from django.db import transaction
from datetime import datetime
from django.views.decorators.csrf import csrf_exempt


@csrf_exempt
@api_view(['POST'])
def place_order(request):
    try:
        data = request.data
        table_number = data['table_number']
        items = data['items']
        
        # Get the table
        table = Table.objects.get(table_number=table_number)
        
        with transaction.atomic():
            # Create new order
            order = Order.objects.create(table=table, order_time=datetime.now(), total_amount=0)

            total_price = 0

            for item in items:
                menu_item = MenuItem.objects.get(id=item['item_id'])
                quantity = item['quantity']
                
                # Create order item
                OrderItem.objects.create(order=order, menu_item=menu_item, quantity=quantity)

                # Update stock
                if menu_item.stock_quantity >= quantity:
                    menu_item.stock_quantity -= quantity
                    menu_item.save()
                else:
                    raise Exception(f"Not enough stock for {menu_item.name}")

                total_price += menu_item.price * quantity

            # Update total order price
            order.total_amount = total_price
            order.save()
        
        return Response({
            'order_id': order.id,
            'status': 'Order placed successfully',
            'estimated_time': '15 minutes'
        }, status=201)

    except Exception as e:
        return Response({'error': str(e)}, status=400)
