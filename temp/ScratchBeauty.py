#!/usr/bin/env python 
import re, requests

SAVE_DIR_PATH = '/home/xuchenyu/mohism/research/py/temp/'
save = lambda url: open(SAVE_DIR_PATH + url[url.rfind('/')+1:], 'wb').write(requests.get(url).content)

if __name__ == '__main__':
    print "Fuck u!!!"
    map(save, ['http:' + url for url in re.findall(ur'<img src=[\'"](//pic.+?)[\'"].+?>', requests.get('http://www.zhihu.com/question/21052148').content)])

