#define GXL_GEDLIB_SHARED
#include "src/env/ged_env.hpp"
#include <cstring>

int main(int argc, char **argv)
{
    std::string dataset;
    std::string path = "";
    ged::Options::EditCosts cost;
    std::string nThreads = "1";
    if (argc < 2)
    {
        throw ged::Error("No dataset specified. Usage: ./test <Letter|mao|alkane|acyclic|pah> <number of threads>");
    }
    if (strcmp(argv[1], "Letter") == 0)
    {
        dataset = "Letter";
        path = "/HIGH";
        cost = ged::Options::EditCosts::LETTER;
    }
    else if (strcmp(argv[1], "mao") == 0)
    {
        dataset = "mao";
        cost = ged::Options::EditCosts::CHEM_1;
    }
    else if (strcmp(argv[1], "alkane") == 0)
    {
        dataset = "alkane";
        cost = ged::Options::EditCosts::CHEM_1;
    }
    else if (strcmp(argv[1], "acyclic") == 0)
    {
        dataset = "acyclic";
        cost = ged::Options::EditCosts::CHEM_1;
    }
    else if (strcmp(argv[1], "pah") == 0)
    {
        dataset = "pah";
        cost = ged::Options::EditCosts::CHEM_1;
    }
    else
    {
        throw ged::Error("Invalid dataset specified. Usage : ./test <Letter|mao|alkane|acyclic>");
    }
    if(argc == 3 && atoi(argv[2]) != 0) nThreads = argv[2]; 
    std::string collection_file("./data/collections/" + dataset + ".xml");
    std::string graph_dir("./data/datasets/" + dataset + path + "/");
    std::cout << "grapg dir: " << graph_dir << "\n";
    std::cout << "collection file: " << collection_file << "\n";

    ged::GEDEnv<ged::GXLNodeID, ged::GXLLabel, ged::GXLLabel> env;
    env.set_edit_costs(cost);
    std::vector<ged::GEDGraph::GraphID> graph_ids(env.load_gxl_graphs(graph_dir, collection_file,
                                                                      ged::Options::GXLNodeEdgeType::LABELED, ged::Options::GXLNodeEdgeType::UNLABELED));
    env.init(ged::Options::InitType::EAGER_WITHOUT_SHUFFLED_COPIES);
    std::string ipfp_options("--threads "+ nThreads + " --initialization-method RANDOM");
    std::cout << "IPFP option: " << ipfp_options << "\n";
    env.set_method(ged::Options::GEDMethod::IPFP, ipfp_options);

    ged::ProgressBar progress(graph_ids.size());
    std::cout << "\rComputing Graph Edit Distance by using algorithm IPFP: " << progress << std::flush;
    double time = 0;
    double totalUpperGED = 0, totalLowerGED = 0;
    for (auto g_id : graph_ids)
    {
        for (auto h_id : graph_ids)
        {
            env.run_method(g_id, h_id);
            totalUpperGED += env.get_upper_bound(g_id, h_id);
            time += env.get_runtime(g_id, h_id);
        }
        progress.increment();
        std::cout << "\rComputing Graph Edit Distance by using algorithm IPFP: " << progress << std::flush;
    }
    std::cout << "\nTotal compute time: " << time << "\n";
    std::cout << "Total number of calculations: " << graph_ids.size() * graph_ids.size() << "\n";
    std::cout << "Average GED upper bound: " << totalUpperGED / (graph_ids.size() * graph_ids.size()) << "\n";
    std::cout << "Average compute time: " << time / (graph_ids.size() * graph_ids.size()) << "\n";
    return 0;
}