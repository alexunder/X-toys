import urllib.request
from urllib.parse import urljoin
from bs4 import BeautifulSoup
import os, ssl
import re
import socket
import requests
from six.moves.urllib.request import urlopen
from six.moves.urllib.error import HTTPError


def download_pdf(link, location, name):
    try:
        response = requests.get(link)
        with open(os.path.join(location, name), 'wb') as f:
            f.write(response.content)
            f.close()
    except HTTPError:
        print('>>> Error 404: cannot be downloaded!\n') 
        raise   
    except socket.timeout:
        print(" ".join(("can't download", link, "due to connection timeout!")) )
        raise



def shorten_title(title):
    m1 = re.search('[[0-9]*]', title)
    m2 = re.search('".*"', title)
    if m1:
        title = m1.group(0)
    if m2:
        title = ' '.join((title, m2.group(0)))   
        return title[:50] + ' [...]'    


def enable_https():
    if (not os.environ.get('PYTHONHTTPSVERIFY', '') and
        getattr(ssl, '_create_unverified_context', None)):
        ssl._create_default_https_context = ssl._create_unverified_context

if __name__ == '__main__':

    enable_https()

    base = 'https://www.ml.cmu.edu/research/data-analysis-projects.html'
    response = urllib.request.urlopen(base)
    html = response.read()
    soup = BeautifulSoup(html, "lxml")
    response.close()

    print(soup.title.text)
    title = shorten_title(soup.title.text)
    print("title=" + str(title))

    urls  = []
    names = []

    for i, link in enumerate(soup.findAll('a')):
        _FULLURL = urljoin(base, link.get('href'))
        if _FULLURL.endswith('.pdf'):
            #print(_FULLURL)
            urls.append(_FULLURL)
            name = soup.select('a')[i].text
            if name == "":
                #print("name is void, continue")
                continue
            #print(name)
            name = name.replace(" [", "")
            name = name.replace("]", "")
            #print(name)
            names.append(name)

    names_urls = zip(names, urls)

    for name, url in names_urls:
        print("Dowloading from " + str(url))
        #rq = urllib2.Request(url)
        res = urllib.request.urlopen(url)
        print("Saving " + str(name))
        #download_pdf(url, "data-analysis-projects", name)
        pdf = open("data-analysis-projects/" + name, 'wb')
        pdf.write(res.read())
        pdf.close()