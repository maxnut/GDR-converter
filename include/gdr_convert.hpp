#pragma once
#include <matjson/msgpack.hpp>

#include <charconv>
#include <cmath>
#include <gdr/gdr.hpp>

namespace gdr {

template<typename ReplayType, typename InputType>
gdr::Result<ReplayType> convert(
    std::span<uint8_t const> data,
    auto&& replayExt,
    auto&& inputExt
) {
    auto res = matjson::parse(std::string_view(reinterpret_cast<char const*>(data.data()), data.size()))
        .orElse([&](auto) { return matjson::msgpack::parse(data); });

    if (!res) {
        return gdr::Err<ReplayType>(std::move(res.unwrapErr().message));
    }

    ReplayType replay;
    auto json = std::move(res).unwrap();

#define GDR_UNWRAP_INTO(var, ...) \
    auto GEODE_CONCAT(res, __LINE__) = __VA_ARGS__; \
    if (GEODE_CONCAT(res, __LINE__).isErr()) \
        return gdr::Err<ReplayType>(std::move(GEODE_CONCAT(res, __LINE__)).unwrapErr()); \
    var = std::move(GEODE_CONCAT(res, __LINE__)).unwrap()

    GDR_UNWRAP_INTO(auto gameVersion, json["gameVersion"].asDouble());
    replay.gameVersion = std::round(gameVersion * 1000.0);

    GDR_UNWRAP_INTO(replay.description, json["description"].asString());
    GDR_UNWRAP_INTO(replay.duration, json["duration"].asDouble());
    GDR_UNWRAP_INTO(replay.botInfo.name, json["bot"]["name"].asString());

    replay.botInfo.version = 1;
    GDR_UNWRAP_INTO(auto botVersion, json["bot"]["version"].asString());
    std::from_chars(botVersion.data(), botVersion.data() + botVersion.size(), replay.botInfo.version);

    GDR_UNWRAP_INTO(replay.levelInfo.id, json["level"]["id"].asUInt());
    GDR_UNWRAP_INTO(replay.levelInfo.name, json["level"]["name"].asString());
    GDR_UNWRAP_INTO(replay.author, json["author"].asString());
    GDR_UNWRAP_INTO(replay.seed, json["seed"].asInt());
    GDR_UNWRAP_INTO(replay.coins, json["coins"].asInt());
    GDR_UNWRAP_INTO(replay.ldm, json["ldm"].asBool());
    replay.platformer = false;

    if (json.contains("framerate")) {
        GDR_UNWRAP_INTO(replay.framerate, json["framerate"].asDouble());
    }

    if constexpr (std::is_invocable_v<decltype(replayExt), matjson::Value&, ReplayType&>) {
        replayExt(json, replay);
    }

    for (auto& inputJson : json["inputs"]) {
        InputType input;
        GDR_UNWRAP_INTO(input.frame, inputJson["frame"].asUInt());
        GDR_UNWRAP_INTO(input.button, inputJson["btn"].asUInt());
        GDR_UNWRAP_INTO(input.player2, inputJson["2p"].asBool());
        GDR_UNWRAP_INTO(input.down, inputJson["down"].asBool());

        if(input.button != 1)
            replay.platformer = true;

        if constexpr (std::is_invocable_v<decltype(inputExt), matjson::Value&, InputType&>) {
            inputExt(inputJson, input);
        }

        replay.inputs.push_back(std::move(input));
    }

#undef GDR_UNWRAP_INTO

    return gdr::Ok<ReplayType>(std::move(replay));
}

template<typename ReplayType, typename InputType>
gdr::Result<ReplayType> convert(std::span<uint8_t const> data) {
    return convert<ReplayType, InputType>(data, nullptr, nullptr);
}

template<typename ReplayType, typename InputType>
gdr::Result<ReplayType> convert(std::span<uint8_t const> data, auto&& replayExt) {
    return convert<ReplayType, InputType>(data, std::forward<decltype(replayExt)>(replayExt), nullptr);
}

}