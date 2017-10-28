#ifndef _ROOM_SCENE_H
#define _ROOM_SCENE_H

#include "SkinBank.h"
#include "TablePile.h"
#include "aux-skills.h"
#include "card.h"
#include "chatwidget.h"
#include "client.h"
#include "clientlogbox.h"
#include "dashboard.h"
#include "photo.h"
#include "qsanbutton.h"
#include "sprite.h"

class Window;
class Button;
class CardContainer;
class GuanxingBox;
class QSanButton;
class QGroupBox;
struct RoomLayout;
class BubbleChatBox;
class ChooseTriggerOrderBox;

#include <QDialog>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMutex>
#include <QSpinBox>
#include <QStack>
#include <QTableWidget>
#include <QTextEdit>
#include <QThread>

class ScriptExecutor : public QDialog
{
    Q_OBJECT

public:
    ScriptExecutor(QWidget *parent);

public slots:
    void doScript();
};

class DeathNoteDialog : public QDialog
{
    Q_OBJECT

public:
    DeathNoteDialog(QWidget *parent);

protected:
    virtual void accept();

private:
    QComboBox *killer, *victim;
};

class DamageMakerDialog : public QDialog
{
    Q_OBJECT

public:
    DamageMakerDialog(QWidget *parent);

protected:
    virtual void accept();

private:
    QComboBox *damage_source;
    QComboBox *damage_target;
    QComboBox *damage_nature;
    QSpinBox *damage_point;

    void fillComboBox(QComboBox *ComboBox);

private slots:
    void disableSource();
};

class KOFOrderBox : public QGraphicsPixmapItem
{
public:
    KOFOrderBox(bool self, QGraphicsScene *scene);
    void revealGeneral(const QString &name);
    void killPlayer(const QString &general_name);

private:
    QSanSelectableItem *avatars[3];
    int revealed;
};

class ReplayerControlBar : public QGraphicsObject
{
    Q_OBJECT

public:
    ReplayerControlBar(Dashboard *dashboard);
    static QString FormatTime(int secs);
    virtual QRectF boundingRect() const;

public slots:
    void setTime(int secs);
    void setSpeed(qreal speed);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    static const int S_BUTTON_GAP = 3;
    static const int S_BUTTON_WIDTH = 25;
    static const int S_BUTTON_HEIGHT = 21;

private:
    QLabel *time_label;
    QString duration_str;
    qreal speed;
};

class TimeLabel : public QGraphicsObject
{
    Q_OBJECT

public:
    TimeLabel();
    virtual QRectF boundingRect() const;
    void initializeLabel();
    void startCounting();

public slots:
    void updateTimerLabel();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    // Widget For Timer Label
    QLabel *time_label;
    QTimer *timer;
};

class RoomScene : public QGraphicsScene
{
    Q_OBJECT

public:
    RoomScene(QMainWindow *main_window);
    ~RoomScene();
    void changeTextEditBackground();
    void adjustItems();
    void showIndicator(const QString &from, const QString &to);
    void showPromptBox();
    static void FillPlayerNames(QComboBox *ComboBox, bool add_none);
    void updateTable();
    inline QMainWindow *mainWindow()
    {
        return main_window;
    }

    void changeTableBg();
    void changeTableBg(const QString &tableBgImage_path);

    inline bool isCancelButtonEnabled() const
    {
        return cancel_button != NULL && cancel_button->isEnabled();
    }
    inline void setGuhuoLog(const QString &log)
    {
        guhuo_log = log;
    }

    bool m_skillButtonSank;

    const QRectF &getTableRect() const
    {
        return m_tableRect;
    }

    void addHeroSkinContainer(ClientPlayer *player, HeroSkinContainer *heroSkinContainer);
    HeroSkinContainer *findHeroSkinContainer(const QString &generalName) const;
    QSet<HeroSkinContainer *> getHeroSkinContainers();

public slots:
    void addPlayer(ClientPlayer *player);
    void removePlayer(const QString &player_name);
    void loseCards(int moveId, QList<CardsMoveStruct> moves);
    void getCards(int moveId, QList<CardsMoveStruct> moves);
    void keepLoseCardLog(const CardsMoveStruct &move);
    void keepGetCardLog(const CardsMoveStruct &move);
    // choice dialog
    void chooseGeneral(const QStringList &generals);
    void chooseSuit(const QStringList &suits);
    void chooseCard(const ClientPlayer *playerName, const QString &flags, const QString &reason, bool handcard_visible, Card::HandlingMethod method, QList<int> disabled_ids);
    void chooseKingdom(const QStringList &kingdoms);
    void chooseOption(const QString &skillName, const QStringList &options);
    void chooseOrder(QSanProtocol::Game3v3ChooseOrderCommand reason);
    void chooseRole(const QString &scheme, const QStringList &roles);
    void chooseDirection();
    void chooseTriggerOrder(const QVariantList &options, bool optional);

    void bringToFront(QGraphicsItem *item);
    void arrangeSeats(const QList<const ClientPlayer *> &seats);
    void toggleDiscards();
    void enableTargets(const Card *card);
    void useSelectedCard();
    void updateStatus(Client::Status oldStatus, Client::Status newStatus);
    void killPlayer(const QString &who);
    void revivePlayer(const QString &who);
    void setDashboardShadow(const QString &who);
    void showServerInformation();
    void surrender();
    void saveReplayRecord();
    void saveReplayRecord(const bool auto_save, const bool network_only = false);
    void makeDamage();
    void makeKilling();
    void makeReviving();
    void doScript();
    void viewGenerals(const QString &reason, const QStringList &names);

    void handleGameEvent(const QVariant &arg);

    void doOkButton();
    void doCancelButton();
    void doDiscardButton();
    void highlightSkillButton(QString skill_name, bool highlight);
    bool isHighlightStatus(Client::Status status);
    void setLordBGM(QString lord = QString());
    void setLordBackdrop(QString lord = QString());

    void doSkinChange(const QString &generalName, int skinIndex);
    inline QPointF tableCenterPos()
    {
        return m_tableCenterPos;
    }

    void showPile(const QList<int> &card_ids, const QString &name, const ClientPlayer *target);
    QString getCurrentShownPileName();
    void hidePile();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    //this method causes crashes
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QMutex m_roomMutex;
    QMutex m_zValueMutex;

private:
    void _getSceneSizes(QSize &minSize, QSize &maxSize);
    bool _shouldIgnoreDisplayMove(CardsMoveStruct &movement);
    bool _processCardsMove(CardsMoveStruct &move, bool isLost);
    bool _m_isMouseButtonDown;
    bool _m_isInDragAndUseMode;
    const QSanRoomSkin::RoomLayout *_m_roomLayout;
    const QSanRoomSkin::PhotoLayout *_m_photoLayout;
    const QSanRoomSkin::CommonLayout *_m_commonLayout;
    const QSanRoomSkin *_m_roomSkin;
    QGraphicsItem *_m_last_front_item;
    double _m_last_front_ZValue;
    GenericCardContainer *_getGenericCardContainer(Player::Place place, Player *player);
    QMap<int, QList<QList<CardItem *> > > _m_cardsMoveStash;
    Button *add_robot, *fill_robots, *return_to_main_menu;
    QList<Photo *> photos;
    QMap<QString, Photo *> name2photo;
    Dashboard *dashboard;
    TablePile *m_tablePile;
    QMainWindow *main_window;
    QSanButton *ok_button, *cancel_button, *discard_button;
    QSanButton *trust_button;
    QMenu *miscellaneous_menu, *change_general_menu;
    Window *prompt_box;
    Window *pindian_box;
    CardItem *pindian_from_card, *pindian_to_card;
    QGraphicsItem *control_panel;
    QMap<PlayerCardContainer *, const ClientPlayer *> item2player;
    QDialog *m_choiceDialog; // Dialog for choosing generals, suits, card/equip, or kingdoms

    QGraphicsRectItem *pausing_item;
    QGraphicsSimpleTextItem *pausing_text;

    QString guhuo_log;

    QList<QGraphicsPixmapItem *> role_items;
    CardContainer *card_container;
    CardContainer *pileContainer;

    QList<QSanSkillButton *> m_skillButtons;

    ResponseSkill *response_skill;
    ShowOrPindianSkill *showorpindian_skill;
    DiscardSkill *discard_skill;
    YijiViewAsSkill *yiji_skill;
    ChoosePlayerSkill *choose_skill;

    QList<const Player *> selected_targets;

    GuanxingBox *guanxing_box;
    ChooseTriggerOrderBox *m_chooseTriggerOrderBox;
    QList<CardItem *> gongxin_items;

    ClientLogBox *log_box;
    QTextEdit *chat_box;
    QLineEdit *chat_edit;
    QGraphicsProxyWidget *chat_box_widget;
    QGraphicsProxyWidget *log_box_widget;
    QGraphicsProxyWidget *chat_edit_widget;
    QGraphicsTextItem *prompt_box_widget;
    ChatWidget *chat_widget;
    QPixmap m_rolesBoxBackground;
    QGraphicsPixmapItem *m_rolesBox;
    QGraphicsTextItem *m_pileCardNumInfoTextBox;
    QGraphicsPixmapItem *m_tableBg;
    QString image_path;
    QString bgm_path;
    int m_tablew;
    int m_tableh;

    TimeLabel *time_label_widget;

    QMap<QString, BubbleChatBox *> m_bubbleChatBoxs;

    // for 3v3 & 1v1 mode
    QSanSelectableItem *selector_box;
    QList<CardItem *> general_items, up_generals, down_generals;
    CardItem *to_change;
    QList<QGraphicsRectItem *> arrange_rects;
    QList<CardItem *> arrange_items;
    Button *arrange_button;
    KOFOrderBox *enemy_box, *self_box;
    QPointF m_tableCenterPos;
    ReplayerControlBar *m_replayControl;

    struct _MoveCardsClassifier
    {
        inline _MoveCardsClassifier(const CardsMoveStruct &move)
        {
            m_card_ids = move.card_ids;
        }
        inline bool operator==(const _MoveCardsClassifier &other) const
        {
            return m_card_ids == other.m_card_ids;
        }
        inline bool operator<(const _MoveCardsClassifier &other) const
        {
            return m_card_ids.first() < other.m_card_ids.first();
        }
        QList<int> m_card_ids;
    };

    QMap<_MoveCardsClassifier, CardsMoveStruct> m_move_cache;

    // @todo: this function shouldn't be here. But it's here anyway, before someone find a better
    // home for it.
    QString _translateMovement(const CardsMoveStruct &move);

    void useCard(const Card *card);
    void fillTable(QTableWidget *table, const QList<const ClientPlayer *> &players);
    void chooseSkillButton();

    void selectTarget(int order, bool multiple);
    void selectNextTarget(bool multiple);
    void unselectAllTargets(const QGraphicsItem *except = NULL);
    void updateTargetsEnablity(const Card *card = NULL);

    void callViewAsSkill();
    void cancelViewAsSkill();

    void freeze();
    void addRestartButton(QDialog *dialog);
    QGraphicsItem *createDashboardButtons();
    void createReplayControlBar();

    void fillGenerals1v1(const QStringList &names);
    void fillGenerals3v3(const QStringList &names);

    void showPindianBox(const QString &from_name, int from_id, const QString &to_name, int to_id, const QString &reason);
    void setChatBoxVisible(bool show);
    QRect getBubbleChatBoxShowArea(const QString &who) const;

    // animation related functions
    typedef void (RoomScene::*AnimationFunc)(const QString &, const QStringList &);
    QGraphicsObject *getAnimationObject(const QString &name) const;

    void doMovingAnimation(const QString &name, const QStringList &args);
    void doAppearingAnimation(const QString &name, const QStringList &args);
    void doLightboxAnimation(const QString &name, const QStringList &args);
    void doHuashen(const QString &name, const QStringList &args);
    void doIndicate(const QString &name, const QStringList &args);
    EffectAnimation *animations;
    bool pindian_success;

    // re-layout attempts
    bool game_started;
    void _dispersePhotos(QList<Photo *> &photos, QRectF disperseRegion, Qt::Orientation orientation, Qt::Alignment align);

    void _cancelAllFocus();
    // for miniscenes
    int _m_currentStage;

    QRectF _m_infoPlane;
    QRectF m_tableRect;
    QSet<HeroSkinContainer *> m_heroSkinContainers;

private slots:
    void fillCards(const QList<int> &card_ids, const QList<int> &disabled_ids = QList<int>());
    void updateSkillButtons();
    void acquireSkill(const ClientPlayer *player, const QString &skill_name);
    void updateSelectedTargets();
    void updateTrustButton();
    void onSkillActivated();
    void onSkillDeactivated();
    void doTimeout();

    void startInXs();
    void hideAvatars();
    void changeHp(const QString &who, int delta, DamageStruct::Nature nature, bool losthp);
    void changeMaxHp(const QString &who, int delta);
    void moveFocus(const QStringList &who, QSanProtocol::Countdown);
    void setEmotion(const QString &who, const QString &emotion);
    void setEmotion(const QString &who, const QString &emotion, bool permanent);
    void showSkillInvocation(const QString &who, const QString &skill_name);
    void doAnimation(int name, const QStringList &args);
    void showOwnerButtons(bool owner);
    void showPlayerCards();
    void updateRolesBox();
    void updateRoles(const QString &roles);
    void addSkillButton(const Skill *skill, bool from_left = false);

    void resetPiles();
    void removeLightBox();

    void showCard(const QString &player_name, int card_id);
    void viewDistance();

    void speak();

    void onGameStart();
    void onGameOver();
    void onStandoff();

    void appendChatEdit(QString txt);
    void appendChatBox(QString txt);
    void showBubbleChatBox(const QString &who, const QString &line);

    //animations
    void onEnabledChange();

    void takeAmazingGrace(ClientPlayer *taker, int card_id, bool move_cards);

    void attachSkill(const QString &skill_name, bool from_left);
    void detachSkill(const QString &skill_name);

    void doGongxin(const QList<int> &card_ids, bool enable_heart, QList<int> enabled_ids);

    void startAssign();

    void doPindianAnimation();

    // 3v3 mode & 1v1 mode
    void fillGenerals(const QStringList &names);
    void takeGeneral(const QString &who, const QString &name, const QString &rule);
    void recoverGeneral(int index, const QString &name);
    void startGeneralSelection();
    void selectGeneral();
    void startArrange(const QString &to_arrange);
    void toggleArrange();
    void finishArrange();
    void changeGeneral(const QString &general);
    void revealGeneral(bool self, const QString &general);

    void skillStateChange(const QString &skill_name);
    void trust();
    void skillInvalidityChange(ClientPlayer *player);

signals:
    void restart();
    void return_to_start();
};

extern RoomScene *RoomSceneInstance;

#endif