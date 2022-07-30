#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include "cumBot.h"
#include <tgbot/tgbot.h>
#include <tgbot/types/BotCommand.h>
#include <tgbot/types/InlineKeyboardButton.h>

using namespace TgBot;

void createKeyboard(const std::vector<std::vector<std::string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb)
{
    for (size_t i = 0; i < buttonLayout.size(); ++i)
    {
        std::vector<KeyboardButton::Ptr> row;
        for (size_t j = 0; j < buttonLayout[i].size(); ++j)
        {
            KeyboardButton::Ptr button(new KeyboardButton);
            button->text = buttonLayout[i][j];
            row.push_back(button);
        }
        kb->keyboard.push_back(row);
    }
}

int main()
{
    std::string token = getenv("TOKEN");
    printf("Token: %s\n", token.c_str());

    const int nSlaves = 4;
    int currentSlave = 0;

    std::vector<std::string> photos = {"../media/bill.jpg", "../media/van.jpg", "../media/rambo.jpg", "../media/ray.jpg", "../media/brad.jpg"};
    const std::string photoMimeType = "image/jpeg";

    bool test_text_state = false;
    bool showCustomReplyLeyboard = false;

    Bot bot(token);

    //Reply keyboards
    ReplyKeyboardMarkup::Ptr keyboardWithLayout(new ReplyKeyboardMarkup);
    createKeyboard( {
                    {"Dog", "Cat", "Mouse"},
                    {"Green", "White", "Red"},
                    {"On", "Off"}
                    },
                    keyboardWithLayout);

    //Commands
    std::vector<BotCommand::Ptr> commands;
    BotCommand::Ptr cmdArray(new BotCommand);
    cmdArray->command = "start";
    cmdArray->description = "Start conversation";

    commands.push_back(cmdArray);

    cmdArray = BotCommand::Ptr(new BotCommand);
    cmdArray->command = "cum";
    cmdArray->description = "embrace the CUM";
    commands.push_back(cmdArray);

    cmdArray = BotCommand::Ptr(new BotCommand);
    cmdArray->command = "switch";
    cmdArray->description = "switch a slave";
    commands.push_back(cmdArray);

    cmdArray = BotCommand::Ptr(new BotCommand);
    cmdArray->command = "photo";
    cmdArray->description = "display a serious photo";
    commands.push_back(cmdArray);

    bot.getApi().setMyCommands(commands);

    // Inline buttons
    InlineKeyboardMarkup::Ptr arrowsKeyboard(new InlineKeyboardMarkup);
    InlineKeyboardMarkup::Ptr arrowsKeyboard1;
    InlineKeyboardButton::Ptr row1[3];
    std::vector<InlineKeyboardButton::Ptr> row0;
    InlineKeyboardButton::Ptr leftButton(new InlineKeyboardButton);
    InlineKeyboardButton::Ptr selectButton(new InlineKeyboardButton);
    InlineKeyboardButton::Ptr rightButton(new InlineKeyboardButton);

    leftButton->text = "<";
    leftButton->callbackData = "left";
    row0.push_back(leftButton);
    
    selectButton->text = "select";
    selectButton->callbackData = "select";
    row0.push_back(selectButton);

    rightButton->text = ">";
    rightButton->callbackData = "right";
    row0.push_back(rightButton);

    arrowsKeyboard->inlineKeyboard.push_back(row0);

    bot.getEvents().onCommand("start", [&bot, &photos, &photoMimeType](Message::Ptr message)
    {
        bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photos[VAN], photoMimeType));
        bot.getApi().sendMessage(message->chat->id, "Welcome to the Dungeon. The master awaits.");
    });

    bot.getEvents().onCommand("switch", [&bot, &arrowsKeyboard, &test_text_state](Message::Ptr message)
    {
        std::string response = "ok";
        bot.getApi().sendMessage(message->chat->id, response, false, 0, arrowsKeyboard, "Markdown");
        test_text_state = true;
    });

    bot.getEvents().onCommand("cum", [&bot](Message::Ptr message)
    {
        std::string response = "There will be CUM!";
        bot.getApi().sendMessage(message->chat->id, response);
    });

    bot.getEvents().onCommand("photo", [&bot, &photos, &keyboardWithLayout, &photoMimeType](Message::Ptr message)
    {
        bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photos[BILL], photoMimeType));
    });

    bot.getEvents().onCallbackQuery
    ([&bot, &arrowsKeyboard](CallbackQuery::Ptr query)
    {
        if (query->data == "left")
        {
            std::string response = "left one";
            bot.getApi().sendMessage(query->message->chat->id, response, false, 0, arrowsKeyboard, "Markdown");
        }
        else if (query->data == "right")
        {
            std::string response = "right one";
            bot.getApi().sendMessage(query->message->chat->id, response, false, 0, arrowsKeyboard, "Markdown");
        }
        else 
        {
            std::string response = "selected";
            bot.getApi().sendMessage(query->message->chat->id, response, false, 0, arrowsKeyboard, "Markdown");
        }
    });

    bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr message)
    {
        if (test_text_state)
        {
            bot.getApi().sendMessage(message->chat->id, message->text);
            test_text_state = false;
            return;
        }
        
        for (const auto& command : commands)
        {
            if (StringTools::startsWith(message->text, "/" + command->command))
            {
                return;
            }
        }
        bot.getApi().sendMessage(message->chat->id, "unknown command");
    });

    signal(SIGINT, [](int s)
    {
        printf("SIGINT got\n");
        exit(0);
    });

    try
    {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true)
        {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (std::exception& e)
    {
        printf("error: %s\n", e.what());
    }

    return 0;
}
