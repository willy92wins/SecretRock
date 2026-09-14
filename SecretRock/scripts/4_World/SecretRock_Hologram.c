modded class Hologram
{
    override string ProjectionBasedOnParent()
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                return deployKit.GetDeployedClassname();
            }
        }

        return super.ProjectionBasedOnParent();
    }

    override string GetProjectionName(ItemBase item)
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                return deployKit.GetDeployedClassname();
            }
        }

        return super.GetProjectionName(item);
    }

    override EntityAI PlaceEntity(EntityAI entity_for_placing)
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                return entity_for_placing;
            }
        }

        return super.PlaceEntity(entity_for_placing);
    }
};
