#!/bin/bash
#create by zyao 20181116 for Ϊ�˸��������������Ŀ�����������������ű�
#����֮ǰҲҪ��ִ�и��ļ�����ΪҪ�����»����������˴�����ʱ���ã�����뷽����Խ���̬��·�����뻷������LIBRARY_PATH�޸���Ŀ¼.bash_profile��.profile
echo "����֮ǰ��ִ�и��ļ�"
CURRENT_DIR=`pwd`
#ָ������̬���ӿ��ļ�����·��
export LIBRARY_PATH=$LIBRARY_PATH:$CURRENT_DIR/lib:.

#�������������������
cd bin
./mssmatch

