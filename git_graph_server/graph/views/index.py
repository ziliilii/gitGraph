from django.shortcuts import render


def index(request):
    #return render(request, "multiends/web.html")
    return render(request, "multiends/gitgraph.html")
