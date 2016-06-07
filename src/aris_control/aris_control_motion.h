#ifndef ARIS_CONTROL_MOTION_H
#define ARIS_CONTROL_MOTION_H

#include <functional>
#include <thread>
#include <atomic>

#include <aris_control_ethercat.h>


namespace aris
{
    namespace control
    {
        struct TxMotionData :public Slave::TxType
        {
            double target_pos{ 0 };
            double target_vel{ 0 };
            double target_tor{ 0 };
            double vel_offset{0};
            double tor_offset{0};

            std::uint8_t cmd{ 0 };
            std::uint8_t mode{ 8 };
            std::int8_t home_mode{35};
        };
        struct RxMotionData :public Slave::RxType
        {
            double feedback_pos{ 0 };
            double feedback_vel{ 0 };
            double feedback_tor{ 0 };
            std::uint8_t cmd{ 0 };
            std::uint8_t mode{ 8 };
            std::int8_t fault_warning{0};
        };
        class Motion :public SlaveTemplate<TxMotionData, RxMotionData>
        {
        public:
            enum error{
                SUCCESS=0,
                EXECUTING=1,
                EXE_FAULT=2,//the fault accure during executing ,can reset
                NOT_START=3,
                NOT_READY=4,
                CMD_ERROR=-1,//all motor should disable when the error accure
                EXE_ERROR=-2,//all motor should halt when the error accure during executing

            };
            enum Cmd
            {
                IDLE = 0,
                ENABLE,
                DISABLE,
                HOME,
                RUN
            };
            enum Mode
            {
                HOME_MODE = 0x0006,
                POSITION = 0x0008,
                VELOCITY = 0x0009,
                TORQUE = 0x0010,
            };

            virtual ~Motion();
            static auto Type()->const std::string &{ static const std::string type("motion"); return std::ref(type); }
            virtual auto type() const->const std::string&{ return Type(); }
            Motion(Object &father, std::size_t id, const aris::core::XmlElement &xml_ele);

            auto txTypeSize()const->std::size_t override{ return sizeof(TxMotionData); }
            auto rxTypeSize()const->std::size_t override{ return sizeof(RxMotionData); }
            auto logData(std::fstream &file, TxType *tx_data, RxType *rx_data)->void;

            auto maxPos()->double;
            auto minPos()->double;
            auto maxVel()->double;
            auto pos2countRatio()->std::int32_t;

        protected:
            virtual auto readUpdate()->void override;
            virtual auto writeUpdate()->void override;

        private:
            class Imp;
            std::unique_ptr<Imp> imp_;
        };

        class Controller:public Master
        {
        public:
            auto setControlStrategy(std::function<void()> strategy)->void { strategy_ = strategy; }
            Controller() { registerChildType<Motion, false, false, false, false>(); }

        protected:
            auto controlStrategy()->void override final { if (strategy_)strategy_(); }

        private:
            std::function<void()> strategy_{nullptr};
        };
    }
}

#endif
