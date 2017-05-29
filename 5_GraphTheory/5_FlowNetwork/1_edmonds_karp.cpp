//Edmonds-Karp算法
//edmonds_karp.cpp

//一座城市(图G)，生产厂为源点beg，存储厂为汇点end
//生产厂拥有无限的制造货物能力，而存储厂拥有无限的存储货物能力
//两个厂之间由一些节点和管道联接
//每个节点的接收和输出能力也是无限且瞬时的，可惜每个管道的运输能力是一定且有限的
//而且每个管道的运输方向也只是单向的
//求存储厂能接收的最大货物量，这个问题即为最大流问题
//
//Edmonds-Karp算法属于增广路径类算法，用该算法求最大流问题

//1)基础概念
//流网络：
//在有向简单图G=<V,E>中，一条权值为正整数的边表示该边具有一定的运输能力
//图G中有源点beg和汇点end，其中源点可以发出无限的流而汇点可以接收无限的流
//限制它们的是从beg到end的路径
//从beg到end的最大运输量即为最大流，或者进入汇点的最大流
//流：
//关于流的更加严格的定义是f(v,u)，v和u为图G中一条边的两端点
//则f(v, u)是从v到u的流，为了方便流的值均为正整数
//容量/残留容量：
//边的权值即为该边上能通过的最大(残余)流量，所剩余的运输能力，即该边的(残余)容量
//残留网络：
//将由具有权值的有向边组成的图成为流网络
//当流网络发生变化，称之为残路网络，原始的流网络也可以看做一个原始的残留网络
//“添加反向边”(也称反向弧)副作用：
//这是流网络中一个重要的副作用
//在图G中，每条边的容量并不一定会被完全使用(当然我们直觉的希望可以尽量使用它)
//设从节点 u 到节点 v 的边e有容量 c，当该边使用了 x 的流(运输了x流量)
//其中 c 和 x 均为正整数
//则需要对当前的残留网络进行更新，得到新的残留网络：
//将边e上的容量 c 减去流量 x，即原边e的容量减小为 c-x(c-x不会为负值)
//但仅仅减去正向边的容量还是不够的
//使用一条边的容量会有一个副作用，即“添加反向边”：
//还需要添加一条从节点 v 到节点 u 的反向边，其容量为 x，若已存在反向边则权值增大 x
//减去正向边，添加反向边，才能构成新的残留网络
//
//第一次接触最大流算法时，读者对更新残留网络的原始想法一般是仅仅减去正向边
//即仅仅将边e的容量减去被使用的流，而“添加反向边”的操作看起来难以理解
//这也是我为什么花了如此多的口舌，如此大的篇幅强调它
//仅仅将边e的容量减去流，这个操作是剩余图的概念
//事实上添加反向边是最大流算法中必不可少的操作
//
//只有进行了添加反向边这个操作，更新的残留网络才能够得到理想的最大流
//
//而这个证明过程极其复杂，我们不去考虑
//有兴趣的读者可以参考文档“网络流入门—用于最大流的Dinic算法”，作者“Szz”
//该文档是我见过的唯一一篇涉及到为什么添加反向边的文档(即使算法导论中也未提及)
//
//“添加反向边”这个副作用会贯穿本节的所有最大流算法
//它与本节的所有算法都有深刻而本质的联系，它是最大流算法的核心
//在很多网络文档中，关于添加反向边有着各种各样的名称和讲解
//算法导论中的描述是在残留网络中添加负权值的边
//经过实际测试本节中使用的“添加反向边”(也称添加反向弧)的方法更加有利于编程
//实现时数据结构上的处理更加清晰
//
//增广路径：
//从源点到汇点的一条简单路径，这条路径可以运输流
//该路径经过的每条边e都有残留容量，其中最小的那个即为该增广路径的残留容量
//将这个残留容量加入最大流中，即使用该路径的残留容量
//将这个残留容量累加入最大流，当找出所有增广路径后最大流即为流网络的最大流
//使用增广路径的副作用就是“添加反向边”
//被使用的增广路径上的每条边都需要执行添加反向边的操作
//割：
//将图G的点集V分为两个部分S和T，S包含源点beg，T包含汇点end
//想象一条线通过S和T之间的那些边(这些边的一端点在S中另一端点在T中)将S和T割开
//割(S,T)的净流定义为：该割上从S到T的边的容量和减从T到S的边的容量和
//割(S,T)的容量定义为：该割上从S到T的边的容量和
//最小割：
//图G的最小割即为图G中所有割中容量最小的割
//图G的任意割的净流都相等，且这个净流的值即为图G的最大流的值，也称为图G的流
//最大流最小割定理：
//流f是具有源点beg和汇点end的图G的一个流，则f是g的最大流
//g关于f的残留网络中不含增广路径
//最大流f的值等于g中最小割的容量
//
//以上所有概念以算法导论书中的定义为准，这里的讲解追求容易理解，并不严格
//
//2)Ford-Fulkerson方法求解最大流问题，该方法是所有增广路径方法的基础
//这个方法重复以下步骤：
//找出当前残留网络中的一条增广路径p(初始时将图G看做一个原始的残留网络)
//使用p上的残留容量，即将这条路径上剩余的所有运输能力都使用掉，残留容量加入最大流中
//使用该路径之后，更新得到新的残留网络
//重复上述操作直到残留网络中不再有增广路径，也就是说再没有剩余的运输能力了
//即可得到最大流(之前步骤中已经累加了所有残余容量)
//
//3)Ford-Fulkerson方法的核心在于求增广路径的方法
//本文介绍一种流行的算法Edmonds-Karp算法(简称EK)，属于增广路径类算法
//它用bfs求出增广路径，还使用了一项图论中的基础技术即路径标号技术
//路径标号技术：
//设置一个path数组，标记一条增广路径，将源点称作根节点
//增广路径上的一条边的两端点中，靠近源点的节点为父节点，靠近汇点的节点为子节点
//path[i]指代增广路径中节点i的父节点下标号
//
//还需要注意在得到一条增广路径后，将它的残留容量累加到最大流中
//当无法继续找出增广路径时，这个最大流即为流网络的最大流

#include "general_head.h"
#include "graph.h"
int bfs_path(graph_matrix residue, int beg, int end, int *path);

int edmonds_karp(graph_matrix residue, int beg, int end)
{//流网络residue有g_cnt个节点，下标从0到g_cnt-1，源点为beg汇点为end
 //流网络中使用数据结构graph_matrix的方式与之前三小节不一样
 //之前三小节中将边的权值视为两端点距离，同一节点权值为0，不相连节点权值为INF
 //但流网络中边的权值代表该边的残余容量，不存在的边或没有剩余运输能力的边的权值为0
 //返回最大流max_flow
	int max_flow(0);
	//path[i]指代一次bfs后得到的增广路径中节点i的父节点下标号
	//path在所有增广路径类算法中都有使用，后面不再特别说明
	int path[MAX];
	//进行一次bfs遍历得到增广路径，若不能得到增广路径返回0
	while(bfs_path(residue, beg, end, path)){
		//得到一条增广路径之后使用它的残留容量
		//volum指代增广路径上可通过的最大流量
		int volum(INF);
		//节点u从汇点沿着path指向的父节点回溯经过增广路径上所有节点最终回到源点
		for(int u = end; u != beg; u = path[u])
			//所有增广路径的边上流最小的即为本次通过的最大流量
			volum = min(volum, residue.g_m[path[u]][u]);
		//更新残余网络
		for(int u = end; u != beg; u = path[u]){
			//正向边的容量减去增广路径的残余容量
			residue.g_m[path[u]][u] -= volum;
			//添加反向边的容量
			residue.g_m[u][path[u]] += volum;
			//考虑一下为什么要添加反向边而不是只简单的减去增广路径？
			//如果只是简单的减去增广路径中的残余容量，其结果往往不是最优的
			//而添加反向边可以得到理想的最大流，其证明极其复杂这里略去
			//所有最大流算法都会进行这样的操作，减去残余容量，增加反向容量
			//有兴趣的读者可以参考文档“网络流入门—用于最大流的Dinic算法”，作者“Szz”
		}
		//将本次增广路径上通过的流加入最大流中
		max_flow += volum;
	}
	return(max_flow);	
}
int bfs_path(graph_matrix residue, int beg, int end, int *path)
{//从源点开始bfs寻找残留网络中的增广路径
 //若仍存在一条可通的增广路径则将其存在path数组中，返回1，否则返回0
	queue<int> q;
	//path[i]为-1表示当前节点i未被访问，因为0是头节点下标号
	memset(path, -1, MAX * sizeof(int));
	q.push(beg);
	while(!q.empty()){
		int p = q.front(); q.pop();
		//bfs终止条件是搜索节点到达汇点，即找到一条增广路径
		if(p == end)
			break;
		for(int i = 0; i < residue.g_cnt; ++ i)
			if(i != beg and path[i] == -1 and residue.g_m[p][i]){
				//bfs的条件是本次访问的节点不是源点，尚未被访问，且有运输能力的边
				q.push(i);
				path[i] = p;
			}
	}
	//若bfs结束时汇点不存在父节点则说明不存在增广路径
	if(path[end] == -1)
		return(0);
	else
		return(1);
}
