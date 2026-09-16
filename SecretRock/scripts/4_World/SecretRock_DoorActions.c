modded class ActionOpenDoors
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (target)
        {
            SecretRock_PlacedRock rock;
            if (Class.CastTo(rock, target.GetObject()))
            {
                if (!SecretRock_PlacedRock.SecretRock_IsOpenTarget(target))
                {
                    return false;
                }
            }
        }

        return super.ActionCondition(player, target, item);
    }
};

modded class ActionCloseDoors
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (target)
        {
            SecretRock_PlacedRock rock;
            if (Class.CastTo(rock, target.GetObject()))
            {
                if (!SecretRock_PlacedRock.SecretRock_IsCloseTarget(target))
                {
                    return false;
                }
            }
        }

        return super.ActionCondition(player, target, item);
    }
};
