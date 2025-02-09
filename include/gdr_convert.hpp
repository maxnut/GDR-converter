#include "json.hpp"

#include <gdr/gdr.hpp>
#include <functional>

namespace gdr {

template<typename ReplayType, typename InputType>
gdr::Result<ReplayType> convert(std::span<uint8_t> data, std::function<void(nlohmann::json&, ReplayType&)> replayExt = nullptr, std::function<void(nlohmann::json&, InputType&)> inputExt = nullptr) {
    ReplayType replay;
	nlohmann::json replayJson;

    replayJson = nlohmann::json::parse(data, nullptr, false);
    if (replayJson.is_discarded()) {
        replayJson = nlohmann::json::from_msgpack(data, true, false);
        if (replayJson.is_discarded())
            return gdr::Err<ReplayType>("Invalid data");
    }

	replay.gameVersion = static_cast<int>(replayJson["gameVersion"].get<float>() * 1000.f);
	replay.description = replayJson["description"];
	replay.duration = replayJson["duration"];
	replay.botInfo.name = replayJson["bot"]["name"];
	replay.botInfo.version = std::stoi(replayJson["bot"]["version"].get<std::string>());
	replay.levelInfo.id = replayJson["level"]["id"];
	replay.levelInfo.name = replayJson["level"]["name"];
	replay.author = replayJson["author"];
	replay.seed = replayJson["seed"];
	replay.coins = replayJson["coins"];
	replay.ldm = replayJson["ldm"];
	replay.platformer = false;
	
	if(replayJson.contains("framerate"))
		replay.framerate = replayJson["framerate"];

    if(replayExt)
        replayExt(replayJson, replay);

	for (nlohmann::json& inputJson : replayJson["inputs"]) {
		InputType input;
		input.frame = inputJson["frame"];
		input.button = inputJson["btn"];
		input.player2 = inputJson["2p"];
		input.down = inputJson["down"];

		if(input.button != 1)
			replay.platformer = true;

        if(inputExt)
            inputExt(inputJson, input);
        
		replay.inputs.push_back(input);
	}
	return gdr::Ok<ReplayType>(replay);
}

}