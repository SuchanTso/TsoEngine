# TsoEngine

# ����git
``` http
https://blog.csdn.net/weixin_44842112/article/details/125696979
```
* ps:��������gitbashҲ���ԣ�ֱ������vs��ʹ�á����ߡ�-�������С�-��������powershell��Ҳ��ʹ��ָ��

# ����vs2022
``` http
https://visualstudio.microsoft.com/zh-hans/free-developer-offers/
```
* �������vs2022����Ϊ��������ϵͰ汾�����û���ʱ��Ҫ�޸����ýű�

# ��ȡ��Ŀ
* 1�����뵽����Ҫ�����Ŀ��Ŀ¼������ָ��
``` bash
git clone git@github.com:Zediakense/TsoEngine.git
```

* 2������buildsystem��֧������ָ�
``` bash
git checkout buildsystem
git pull
```
* ps: git checkout ָ�����л���֧������;��ÿ�������Լ��Ŀ�����֧����ɹ��ܣ���ɺ�ͳһ�ϻ�develop��֧
* ��δ����Լ��ķ�֧���ں�������

* ��ʼ����ģ��
``` bash
git submodule update --init --recursive
```

��ָ����Ϊ�˸�����ģ�飬��һ�λ�ȡ��Ŀʱ��Ҫ���У�����ʱ���������ֻ����ͷ�ļ��Ҳ�����Ҳ���������¼�����ģ�飬��Ҫ����

``` bash
git submodule update 
```

# ���û���
* ��ȡ���Ĺ����ļ�������ֱ��˫��make.bat�ļ�һ������TsoEngine.sln
* ���˴�TsoEngine.slnӦ�ÿ��Ա�������



# ��������
* 1�������޷��������ⲿ���ţ�__imp__strncpy
	* ������鿴premake5.lua�ļ��£�����buildoption����ע��ȥ����ps��lua�ű���ע�ͷ����ǡ�--����
* 2�������Ҳ���<unistd.h>
	* �����ȫ����unistd.h����ʵ��sandbox.cpp�����#include <unistd.h>ע�͵�	
* 3��������е�����xxx������Ч�ĳ���
	* ������ҵ��ҷ����ļ������Ҽ����Sandboxѡ����Ϊ������Ŀ��

