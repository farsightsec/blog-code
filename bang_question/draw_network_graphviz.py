### Digraph is for directed graphs
from graphviz import Digraph
import numpy as np

def draw_the_graph(read_data, myfilespec10):
    record_count = read_data.count('\n')

    # Create a directed graph
    g = Digraph(comment='RRname Structure', filename=myfilespec10,
       engine='sfdp', 
       node_attr={'color':'lightblue2','style':'filled',
           'shape':'box', 'fontname':'Arial', 'fontsize':'48'},
       edge_attr={'fontname':'Arial','fontsize':'48'},
       graph_attr={'size':'17', 'ratio':'1', 
           'fontname':'Arial', 'fontsize':'48'})
    
    # begin by creating the list of unique vertices
    mynodes = {}

    source_nodes = []
    source_nodes_label = []
    dest_nodes = []
    dest_nodes_label = []
    edge_types = []
    edge_types_label = []
   
    my_individual_lines = [] 
    my_individual_lines=read_data.split("\n")
    for i in range(0,record_count):
        fields=my_individual_lines[i].split(" ")
        if len(fields)==3:
            # remove spaces between rdata elements in fields[2]
            fields[2]=fields[2].replace(" ","")
            # remove square braces, too
            fields[2]=fields[2].replace("[","")
            fields[2]=fields[2].replace("]","")

            # add a dest node to the list of vertices
            mynodes[i] = fields[2]
        
            # now let's build three lists: sources, destinations and edge_types
            source_nodes_label.append(fields[0])
            source_nodes.append("n"+str(i))

            dest_nodes_label.append(fields[2])
            dest_nodes.append("n"+str(i))

            edge_types_label.append(fields[1])
            edge_types.append("n"+str(i))

    mynode_count=len(dest_nodes)

    for i in range(0,mynode_count):
        g.node(mynodes[i])
#####     , label=mynodes.get(i))
    
    # now let's create the edges
    for i in range(0,mynode_count):
        g.edge(source_nodes_label[i],dest_nodes_label[i],
            label=edge_types_label[i])
    g.render()

#####    g.view()
