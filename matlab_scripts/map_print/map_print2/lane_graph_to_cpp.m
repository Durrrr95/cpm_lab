function lane_graph_to_cpp(lane_graph)
    
    f = fopen('lane_graph.cpp','w');
    
    fprintf(f, '    // Generated by map_print/map_print2/lane_graph_to_cpp.m\n');
    fprintf(f, '    // Do not edit\n');
    fprintf(f, '    struct LaneGraph {\n');
    
    fprintf(f, '        const size_t n_nodes = %i;\n', size(lane_graph.nodes, 1));
    
    fprintf(f, '        const std::vector<double> nodes_x = ');
    write_double_vector(f,lane_graph.nodes(:,1));
    fprintf(f, ';\n');
    
    fprintf(f, '        const std::vector<double> nodes_y = ');
    write_double_vector(f,lane_graph.nodes(:,2));
    fprintf(f, ';\n');
    
    fprintf(f, '        const std::vector<double> nodes_cos = ');
    write_double_vector(f,lane_graph.nodes(:,3));
    fprintf(f, ';\n');
    
    fprintf(f, '        const std::vector<double> nodes_sin = ');
    write_double_vector(f,lane_graph.nodes(:,4));
    fprintf(f, ';\n');
    
    
    fprintf(f, '        const size_t n_edges = %i;\n', length(lane_graph.edges));
    fprintf(f, '        const size_t n_edge_path_nodes = 50;\n');
    
    fprintf(f, '        const std::vector<size_t> edges_start_index = ');
    write_index_vector(f,[lane_graph.edges.start_node_index]);
    fprintf(f, ';\n');
    
    fprintf(f, '        const std::vector<size_t> edges_end_index = ');
    write_index_vector(f,[lane_graph.edges.end_node_index]);
    fprintf(f, ';\n');
    fprintf(f, '\n');
    %    std::vector<std::vector<double>>
    
    col_names = {'x', 'y', 'cos', 'sin'};
    
    for k = 1:4
        fprintf(f, '        const std::vector<std::vector<double>> edges_%s = std::vector<std::vector<double>>\n', col_names{k});
        fprintf(f, '        {\n');
        for i = 1:length(lane_graph.edges)
            n = size(lane_graph.edges(i).path,1);
            slice = round(linspace(1,n,50));
            fprintf(f, '            ');
            write_double_vector(f,lane_graph.edges(i).path(slice,k));
            fprintf(f, ',\n');
        end
        fprintf(f, '        };\n\n');
    end
    
    fprintf(f, '    };\n');
    fclose(f);
    
    

end


function write_double_vector(f,v)

    fprintf(f, 'std::vector<double>{');
    fprintf(f, '%.4e,', v);
    fprintf(f, '}');

end


function write_index_vector(f,v)

    fprintf(f, 'std::vector<size_t>{');
    fprintf(f, '%i,', v);
    fprintf(f, '}');

end