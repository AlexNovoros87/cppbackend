#pragma once

#include "hotdog.h"
#include "result.h"

// Функция-обработчик операции приготовления хот-дога
using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;


struct OrderPack{
 net::io_context* io;
 HotDogHandler handler;   
 std::shared_ptr<GasCooker> gas_cooker;
 Store* store;
};


class Order : public std::enable_shared_from_this<Order> {
private:
std::atomic_int id_{0};
Timer baker_;
Timer burner_; 

std::shared_ptr<Sausage> sausage_ = nullptr;
std::shared_ptr<Bread> bread_ = nullptr;

OrderPack resourses_;
net::strand<net::io_context::executor_type> strand_;

public:
Order(int id, OrderPack pack) : 
                        id_{id},
                        baker_{*(pack.io)}, 
                        burner_{*(pack.io)}, 
                        sausage_(pack.store->GetSausage()),
                        bread_ (pack.store->GetBread()),
                        strand_(net::make_strand(*(pack.io))),
                        resourses_ (std::move(pack)){}

  void MakeOrder(){
    Burn();
    Bake();
  }

private:
void Burn() {
            sausage_->StartFry(*(resourses_.gas_cooker), [self = shared_from_this()](){
            self->burner_.expires_after(HotDog::MIN_SAUSAGE_COOK_DURATION);        
            auto foo = [self](error_code ec){ self->sausage_->StopFry();
            self->CheckOrderReady();
            };
            self->burner_.async_wait( net::bind_executor(self->strand_, std::move(foo)));
       }); 
       
       }

void Bake() {     
       bread_->StartBake(*(resourses_.gas_cooker), [self = shared_from_this()](){
            self->baker_.expires_after(HotDog::MIN_BREAD_COOK_DURATION);        
            auto foo = [self](error_code ec){ self->bread_->StopBaking();
            self->CheckOrderReady();} ;
            self->baker_.async_wait( net::bind_executor(self->strand_, std::move(foo)));
            
       }); }


  bool IsOrderReady(){
   return (sausage_->IsCooked() && bread_->IsCooked());
  }

  void CheckOrderReady(){
    if(IsOrderReady()){
        HotDog hd(id_, sausage_, bread_);
        resourses_.handler(Result<HotDog>(std::move(hd)));
  };}
};

