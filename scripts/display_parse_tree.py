#!/usr/bin/env python
import json
import pydot

def build_tree(program, parent = None, graph = None):
	if(graph == None):
		graph = pydot.Dot()
	else:
		temp = pydot.Cluster()
		graph.add_subgraph(temp)
		graph = temp
	name = program[0]
	rest = program[1:]
	node = pydot.Node(name)
	graph.add_node(node)
	if(parent != None):
		graph.add_edge(pydot.Edge(parent, node))
	for i in rest:
		build_tree(i, node, graph)
	return graph
def main():
	"""
	graph = pydot.Dot()
	first = pydot.Node("0");
	nodes = []
	for i in range(0, 4):
		node = pydot.Node(str(i))
		nodes.append(node)

	for i, node in enumerate(nodes):
		graph.add_node(node)
		edge = pydot.Edge(nodes[i - 1 % len(nodes)], node)
		graph.add_edge(edge)

	"""
	f = open("parse.tree", "r")
	out = ""
	for i in f:
		out += i
	out = out.replace(",]","]")[:-1]
	program = json.loads(out)
	graph = build_tree(program)
	graph.write_png("out.png")

if __name__ == "__main__":
	main()
