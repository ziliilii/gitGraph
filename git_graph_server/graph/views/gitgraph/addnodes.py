from django.http import JsonResponse

def handle(request):
    data = request.GET
    cur_name = data.get('cur_name')
    cur_index = data.get('cur_index')
    cur_commit_ish = data.get('cur_commit_ish')
    length = data.get('length')
    print("***************************************")
    print(cur_name)
    print(cur_index)
    print(cur_commit_ish)
    print(length)
    print("***************************************")
    name = "#" + cur_name
    commit_ish = "a" + cur_commit_ish
    index = length
    return name, commit_ish, index

def add_nodes(request):
    name, commit_ish, index = handle(request)
    return JsonResponse({
        'result': "success",
        'name': name,
        'commit_ish': commit_ish,
        'index': index
    })
