cedit
=====

C言語によるマルチプラットフォームなエディタ  
Windowsベースのコマンドで使いやすさを重視

ご意見・ご要望やコメントは[@kawatea03](http://twitter.com/kawatea03)まで

ダウンロード
------------
_Version 1.3.2_

Windowsの方は[こちら](http://sourceforge.jp/downloads/users/3/3887/cedit-1.3.2.msi/)  
Linux/Macの方は[こちら](http://sourceforge.jp/downloads/users/3/3886/cedit-1.3.2.tar.gz/)

インストール
------------
### Windows ###
ダウンロードしたインストーラを使うだけ

### Linux/Mac ###
`libgtk2.0-dev`と`libgtksourceview2.0-dev`が必要

    $ make
    $ make install

主な機能
--------
* さまざまな言語に対応したシンタックスハイライト
* 改行時や括弧の挿入時に自動でインデント
* 対応する括弧に色づけ
* C言語とC++の構文を自動でチェック(flymake)
* コマンド１つで登録したライブラリを呼び出し
* 複数の文字コードに対応(JIS, SHIFT-JIS, EUC-JP, UTF-8)

日本語の文字化けについて
------------------------
Windowsでは使用するフォントによって日本語が文字化けすることがあります  
ceditをインストールしたフォルダの下にある`etc/pango/pango.aliases`をいじることで直せます  

   使いたいフォント = "使いたいフォント,日本語フォント"

のような行を追加してください

コピーライト
------------
Copyright (c) 2012-2014 kawatea
