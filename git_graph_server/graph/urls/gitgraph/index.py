from django.urls import path
from game.views.gitgraph.addnodes import add_nodes


urlpatterns = [
    path("addnodes/", add_nodes, name="gitgraph_addnodes"),
]
