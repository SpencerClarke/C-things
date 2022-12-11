import xml.dom.minidom
import sys

document = xml.dom.minidom.parse(sys.argv[1])

out = open("temp.txt", "a")
for node in document.getElementsByTagName('div'):
    kana = ""
    kanji = ""
    if not node.hasAttribute('class'):
        continue
    if node.getAttribute('class') != 'jukugo':
        continue
    if node.hasChildNodes():
        for child in node.childNodes:
            if child.nodeType == child.TEXT_NODE:
                kana += child.nodeValue.strip()
                kanji += child.nodeValue.strip()
                continue
            if not child.tagName == "div":
                continue
            if not child.hasAttribute('class'):
                continue
            if child.getAttribute('class') == 'furigana':
                for text in child.childNodes:
                    if text.nodeType == text.TEXT_NODE:
                        kana = text.nodeValue
            elif child.getAttribute('class') == 'f_kanji':
                for text in child.childNodes:
                    if text.nodeType == text.TEXT_NODE:
                        kanji = text.nodeValue
    if kana != "" and kanji != "":
        out.write("{0}/{1}\n".format(kana, kanji))

out.close()
